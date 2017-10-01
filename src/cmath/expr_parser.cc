// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <cmath/expr_parser.h>

namespace cmath {

ExprParser::ExprParser()
    : expression_(), currentPosition_(), currentNumber_(), currentSymbol_(),
      currentToken_() {}

Result<std::unique_ptr<Expr>> ExprParser::parse(const std::string& expression) {
  expression_ = expression;
  currentPosition_ = expression_.begin();
  nextToken();

  try {
    if (auto e = addExpr(); eof())
      return e;
    else
      return make_error_code(UnexpectedToken);
  } catch (std::error_code ec) {
    return ec;
  }
}

std::unique_ptr<Expr> ExprParser::expr() {
  return addExpr();
}

std::unique_ptr<Expr> ExprParser::addExpr() {
  auto lhs = mulExpr();
  while (!eof()) {
    switch (currentToken()) {
      case Token::Plus:
        nextToken();
        lhs = std::make_unique<PlusExpr>(std::move(lhs), mulExpr());
        break;
      case Token::Minus:
        nextToken();
        lhs = std::make_unique<MinusExpr>(std::move(lhs), mulExpr());
        break;
      default:
        return lhs;
    }
  }
  return lhs;
}

std::unique_ptr<Expr> ExprParser::mulExpr() {
  auto lhs = powExpr();
  while (!eof()) {
    switch (currentToken()) {
      case Token::Mul:
        nextToken();
        lhs = std::make_unique<MulExpr>(std::move(lhs), powExpr());
        break;
      case Token::Div:
        nextToken();
        lhs = std::make_unique<DivExpr>(std::move(lhs), powExpr());
        break;
      default:
        return lhs;
    }
  }
  return lhs;
}

std::unique_ptr<Expr> ExprParser::powExpr() {
  auto lhs = primaryExpr();
  while (!eof()) {
    switch (currentToken()) {
      case Token::Pow:
        nextToken();
        lhs = std::make_unique<PowExpr>(std::move(lhs), primaryExpr());
        break;
      default:
        return lhs;
    }
  }
  return lhs;
}

std::unique_ptr<Expr> ExprParser::primaryExpr() {
  switch (currentToken()) {
    case Token::RndOpen: {
      nextToken();  // skip '('
      auto e = expr();
      consumeToken(Token::RndClose);
      return e;
    }
    case Token::Number: {
      auto e = std::make_unique<NumberExpr>(currentNumber_);
      nextToken();
      return e;
    }
    case Token::Symbol: {
      auto e = std::make_unique<SymbolExpr>(currentSymbol_);
      nextToken();
      return e;
    }
    default:
      throw make_error_code(UnexpectedToken);
  }
}

void ExprParser::consumeToken(Token t) {
  if (currentToken() != t)
    throw make_error_code(UnexpectedToken);

  nextToken();
}

ExprParser::Token ExprParser::currentToken() {
  return currentToken_;
}

ExprParser::Token ExprParser::nextToken() {
  if (currentPosition_ == expression_.end())
    return currentToken_ = Token::Eof;

  switch (*currentPosition_) {
    case '+':
      currentPosition_++;
      return currentToken_ = Token::Plus;
    case '-':
      currentPosition_++;
      return currentToken_ = Token::Minus;
    case '*':
      currentPosition_++;
      return currentToken_ = Token::Mul;
    case '/':
      currentPosition_++;
      return currentToken_ = Token::Div;
    case '^':
      currentPosition_++;
      return currentToken_ = Token::Pow;
    case '(':
      currentPosition_++;
      return currentToken_ = Token::RndOpen;
    case ')':
      currentPosition_++;
      return currentToken_ = Token::RndClose;
    default:
      break;
  }

  // variables
  if (std::isalpha(*currentPosition_)) {
    currentSymbol_ = *currentPosition_;
    currentPosition_++;
    return currentToken_ = Token::Symbol;
  }

  // decimal numbers
  if (std::isdigit(*currentPosition_)) {
    currentNumber_ = *currentPosition_ - '0';
    currentPosition_++;
    while (!eof() && std::isdigit(*currentPosition_)) {
      currentNumber_ *= 10;
      currentNumber_ += *currentPosition_ - '0';
      currentPosition_++;
    }
    return currentToken_ = Token::Number;
  }

  throw make_error_code(UnexpectedCharacter);
}

// ----------------------------------------------------------------------------
// ErrorCategory

const ExprParser::ErrorCategory& ExprParser::ErrorCategory::get() {
  static ErrorCategory c;
  return c;
}

const char* ExprParser::ErrorCategory::name() const noexcept {
  return "ExprParserError";
}

std::string ExprParser::ErrorCategory::message(int ec) const {
  switch (static_cast<ErrorCode>(ec)) {
    case UnexpectedCharacter:
      return "Unexpected character";
    case UnexpectedToken:
      return "Unexpected token";
    case UnexpectedEof:
      return "Unexpected end of expression";
  }
}

}  // namespace cmath

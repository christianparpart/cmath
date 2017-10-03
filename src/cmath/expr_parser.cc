// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <cmath/expr_parser.h>

namespace cmath {

ExprTokenizer::ExprTokenizer(const std::string& expression)
    : expression_(expression), currentChar_(expression_.begin()), currentToken_() {}

ExprTokenizer::ExprTokenizer() : ExprTokenizer("") {}

bool ExprTokenizer::eof() const {
  return currentChar_ == expression_.end();
}

bool ExprTokenizer::next() {
  if (currentChar_ == expression_.end()) {
    currentToken_.setToken(Token::Eof);
    return false;
  }

  switch (*currentChar_) {
    case '+':
      currentChar_++;
      currentToken_.setToken(Token::Plus);
      return true;
    case '-':
      currentChar_++;
      currentToken_.setToken(Token::Minus);
      return true;
    case '*':
      currentChar_++;
      currentToken_.setToken(Token::Mul);
      return true;
    case '/':
      currentChar_++;
      currentToken_.setToken(Token::Div);
      return true;
    case '^':
      currentChar_++;
      currentToken_.setToken(Token::Pow);
      return true;
    case '(':
      currentChar_++;
      currentToken_.setToken(Token::RndOpen);
      return true;
    case ')':
      currentChar_++;
      currentToken_.setToken(Token::RndClose);
      return true;
    case ':':
      currentChar_++;
      if (!eof() && *currentChar_ == '=') {
        currentChar_++;
        currentToken_.setToken(Token::Define);
        return true;
      } else {
        throw make_error_code(ExprParser::UnexpectedCharacter);
      }
    case '<':
      // < <= <=>
      currentChar_++;
      if (!eof() && *currentChar_ == '=') {
        currentChar_++;
        if (!eof() && *currentChar_ == '>') {
          currentChar_++;
          currentToken_.setToken(Token::Equivalence);
        } else {
          currentToken_.setToken(Token::LessEqu);
        }
      } else {
        currentToken_.setToken(Token::Less);
      }
      return true;
    case '>':
      // > >=
      if (!eof() && *currentChar_ == '=') {
        currentChar_++;
        currentToken_.setToken(Token::GreaterEqu);
      } else {
        currentToken_.setToken(Token::Greater);
      }
      return true;
    default:
      break;
  }

  // variables
  if (std::isalpha(*currentChar_)) {
    currentToken_.setSymbol(*currentChar_);
    currentChar_++;
    return true;
  }

  // decimal numbers
  if (std::isdigit(*currentChar_)) {
    Number n = *currentChar_ - '0';
    currentChar_++;
    while (!eof() && std::isdigit(*currentChar_)) {
      n *= 10;
      n += *currentChar_ - '0';
      currentChar_++;
    }
    currentToken_.setNumber(n);
    return true;
  }

  throw make_error_code(ExprParser::UnexpectedCharacter);
}

ExprParser::ExprParser() : expression_(), currentToken_() {}

Result<std::unique_ptr<Expr>> ExprParser::parse(const std::string& expression) {
  expression_ = expression;
  currentToken_ = ExprTokenizer(expression);
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
      nextToken();
      auto e = expr();
      consumeToken(Token::RndClose);
      return e;
    }
    case Token::Minus: {
      nextToken();
      return std::make_unique<NegExpr>(primaryExpr());
    }
    case Token::Number: {
      auto e = std::make_unique<NumberExpr>(currentToken_->number());
      nextToken();
      return e;
    }
    case Token::Symbol: {
      auto e = std::make_unique<SymbolExpr>(currentToken_->symbol());
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

Token ExprParser::currentToken() {
  return currentToken_->token();
}

Token ExprParser::nextToken() {
  currentToken_++;
  return currentToken_->token();
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

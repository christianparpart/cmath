// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <cmath/expr_parser.h>
#include <codecvt>
#include <iostream>
#include <locale>

// TODO: skip spaces in ExprTokenizer

namespace cmath {

inline std::string toUtf8(const std::u16string& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(s);
}

inline std::u16string toUtf16(const std::string& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(s);
}

inline std::string toUtf8(char16_t ch) {
  std::u16string s;
  s += ch;
  return toUtf8(s);
}

ExprTokenizer::ExprTokenizer(const std::u16string& expression)
    : expression_(expression), currentChar_(expression_.cbegin()), currentToken_() {}

ExprTokenizer::ExprTokenizer() : ExprTokenizer(std::u16string()) {}

bool ExprTokenizer::eof() const {
  return currentToken_.token() == Token::Eof;
}

inline bool isGreekLetter(char16_t ch) {
  // capital letters
  if (ch >= 913 && ch <= 937)
    return true;

  // small letters
  if (ch >= 945 && ch <= 969)
    return true;

  // cursives and archaic
  if (ch >= 976 && ch <= 993)
    return true;

  return false;
}

std::ostream& operator<<(std::ostream& os, Token t) {
  switch (t) {
    case Token::Eof:
      return os << "Eof";
    case Token::Number:
      return os << "Number";
    case Token::Symbol:
      return os << "Symbol";
    case Token::Equ:
      return os << "Equ";
    case Token::NotEqu:
      return os << "NotEqu";
    case Token::LessEqu:
      return os << "LessEqu";
    case Token::GreaterEqu:
      return os << "GreaterEqu";
    case Token::Less:
      return os << "Less";
    case Token::Greater:
      return os << "Greater";
    case Token::Plus:
      return os << "Plus";
    case Token::Minus:
      return os << "Minus";
    case Token::Mul:
      return os << "Mul";
    case Token::Div:
      return os << "Div";
    case Token::Pow:
      return os << "Pow";
    case Token::Fac:
      return os << "Fac";
    case Token::RndOpen:
      return os << "RndOpen";
    case Token::RndClose:
      return os << "RndClose";
    case Token::Define:
      return os << "Define";
    case Token::Equivalence:
      return os << "Equivalence";
  }
}

bool ExprTokenizer::next() {
  while (currentChar_ != expression_.end() && std::isspace(*currentChar_))
    currentChar_++;

  if (currentChar_ == expression_.end()) {
    currentToken_.setToken(Token::Eof);
    return false;
  }

  // std::cout << "next: currentChar: "
  //           << toUtf8(*currentChar_)
  //           << " " << "currentToken: " << currentToken_.token()
  //           << '\n';

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
      printf(":\n");
      if (currentChar_ != expression_.end()  && *currentChar_ == '=') {
        printf("=\n");
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
      if (currentChar_ != expression_.end() && *currentChar_ == '=') {
        currentChar_++;
        currentToken_.setToken(Token::GreaterEqu);
      } else {
        currentToken_.setToken(Token::Greater);
      }
      return true;
    default:
      break;
  }

  // decimal numbers
  if (std::isdigit(*currentChar_)) {
    Number n = *currentChar_ - '0';
    currentChar_++;
    while (currentChar_ != expression_.end()  && std::isdigit(*currentChar_)) {
      n *= 10;
      n += *currentChar_ - '0';
      currentChar_++;
    }
    currentToken_.setNumber(n);
    return true;
  }

  // variables
  if (isalpha(*currentChar_) || isGreekLetter(*currentChar_)) {
    std::u16string v;
    v += *currentChar_++;
    currentToken_.setSymbol(toUtf8(v));
    return true;
  }

  throw make_error_code(ExprParser::UnexpectedCharacter);
}

ExprParser::ExprParser(const std::string& e) : ExprParser(toUtf16(e)) {}

ExprParser::ExprParser(const std::u16string& e) : expression_(e), currentToken_(e) {
  nextToken();
}

ExprTokenizer& ExprTokenizer::operator=(const ExprTokenizer& t) {
  expression_ = t.expression_;
  currentChar_ = std::next(expression_.cbegin(), t.offset());
  currentToken_ = t.currentToken_;

  return *this;
}

std::string ExprTokenizer::expression() const {
  return toUtf8(expression_);
}

std::ostream& operator<<(std::ostream& os, const ExprTokenizer& t) {
  os << "T{" << std::distance(t.expression_.begin(), t.currentChar_) << ", \""
     << t.expression() << "\"}";
  return os;
}

Result<std::unique_ptr<Expr>> parseExpression(const std::string& expression) {
  return ExprParser(expression).parse();
}

Result<std::unique_ptr<Expr>> parseExpression(const std::u16string& expression) {
  return ExprParser(expression).parse();
}

Result<std::unique_ptr<Expr>> ExprParser::parse() {
  try {
    if (auto e = addExpr(); currentToken_.eof()) {
      return e;
    } else {
      return make_error_code(UnexpectedToken);
    }
  } catch (std::error_code ec) {
    return ec;
  }
}

std::unique_ptr<Expr> ExprParser::expr() {
  return relExpr();
}

std::unique_ptr<Expr> ExprParser::relExpr() {
  auto lhs = addExpr();
  while (!currentToken_.eof()) {
    switch (currentToken()) {
      case Token::Define:
        printf("define:\n");
        nextToken();
        lhs = std::make_unique<DefineExpr>(std::move(lhs), addExpr());
        break;
      case Token::Equ:
        nextToken();
        lhs = std::make_unique<EquExpr>(std::move(lhs), addExpr());
        break;
      default:
        return lhs;
    }
  }
  return lhs;
}

std::unique_ptr<Expr> ExprParser::addExpr() {
  auto lhs = mulExpr();
  while (!currentToken_.eof()) {
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
  for (;;) {
    switch (currentToken()) {
      case Token::Eof:
      case Token::RndClose:
        return lhs;
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
        // // a*b = ab
        // std::cout << "x: " << currentToken() << '\n';
        // lhs = std::make_unique<MulExpr>(std::move(lhs), powExpr());
        // break;
    }
  }
  return lhs;
}

std::unique_ptr<Expr> ExprParser::powExpr() {
  auto lhs = primaryExpr();
  while (!currentToken_.eof()) {
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

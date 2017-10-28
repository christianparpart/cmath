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

void ExprToken::setToken(Token t) {
  token_ = t;

  //   switch (token_) {
  //     case Token::Number:
  //       std::cout << "Token: " << t << number_ << std::endl;
  //       break;
  //     case Token::Symbol:
  //       std::cout << "Token: " << t << symbol_ << std::endl;
  //       break;
  //     default:
  //       std::cout << "Token: " << t << std::endl;
  //       break;
  //   }
}

void ExprToken::setSymbol(const Symbol& s) {
  symbol_ = s;
  setToken(Token::Symbol);
}

void ExprToken::setNumber(Number n) {
  number_ = n;
  setToken(Token::Number);
}

ExprTokenizer::ExprTokenizer(std::u16string::const_iterator begin,
                             std::u16string::const_iterator end)
    : currentChar_(begin), endChar_(end), currentToken_() {}

ExprTokenizer::ExprTokenizer()
    : ExprTokenizer(std::u16string::const_iterator(), std::u16string::const_iterator()) {}

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
    case Token::Comma:
      return os << "comma";
    case Token::Colon:
      return os << ":";
    case Token::RightArrow:
      return os << "->";
  }
}

bool ExprTokenizer::next() {
  while (hasBytesPending() && std::isspace(*currentChar_))
    currentChar_++;

  if (!hasBytesPending()) {
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
      if (hasBytesPending() && *currentChar_ == '>') {
        currentChar_++;
        currentToken_.setToken(Token::RightArrow);
      } else {
        currentToken_.setToken(Token::Minus);
      }
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
    case '!':
      currentChar_++;
      currentToken_.setToken(Token::Fac);
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
      if (hasBytesPending() && *currentChar_ == '=') {
        currentChar_++;
        currentToken_.setToken(Token::Define);
        return true;
      } else {
        currentToken_.setToken(Token::Colon);
        return true;
      }
    case '<':
      // < <> <= <=>
      currentChar_++;
      if (!hasBytesPending()) {  // <
        currentToken_.setToken(Token::Less);
      } else if (*currentChar_ == '>') {  // <>
        currentChar_++;
        currentToken_.setToken(Token::NotEqu);
      } else if (*currentChar_ == '=') {
        currentChar_++;
        if (!hasBytesPending() && *currentChar_ == '>') {  // <=>
          currentChar_++;
          currentToken_.setToken(Token::Equivalence);
        } else {  // <=
          currentToken_.setToken(Token::LessEqu);
        }
      } else {  // <
        currentToken_.setToken(Token::Less);
      }
      return true;
    case '>':
      // > >=
      if (hasBytesPending() && *currentChar_ == '=') {
        currentChar_++;
        currentToken_.setToken(Token::GreaterEqu);
      } else {
        currentToken_.setToken(Token::Greater);
      }
      return true;
    case '=':
      currentChar_++;
      currentToken_.setToken(Token::Equ);
      return true;
    case ',':
      currentChar_++;
      currentToken_.setToken(Token::Comma);
      return true;
    default:
      break;
  }

  // decimal numbers
  if (std::isdigit(*currentChar_)) {
    Number n = *currentChar_ - '0';
    currentChar_++;
    while (hasBytesPending() && std::isdigit(*currentChar_)) {
      n *= 10;
      n += *currentChar_ - '0';
      currentChar_++;
    }
    currentToken_.setNumber(n);
    return true;
  }

  // greek letters as symbols
  if (isGreekLetter(*currentChar_)) {
    std::u16string v;
    v += *currentChar_++;
    currentToken_.setSymbol(toUtf8(v));
    return true;
  }

  // latin multi-letter symbols
  if (isalpha(*currentChar_)) {
    std::u16string v;
    do
      v += *currentChar_++;
    while (hasBytesPending() && isalpha(*currentChar_));
    currentToken_.setSymbol(toUtf8(v));
    return true;
  }

  throw make_error_code(ExprParser::UnexpectedCharacter);
}

ExprParser::ExprParser(const SymbolTable& symbolTable, const std::string& e)
    : ExprParser(symbolTable, toUtf16(e)) {}

ExprParser::ExprParser(const SymbolTable& symbolTable, const std::u16string& e)
    : symbolTable_(symbolTable), expression_(e),
      currentToken_(expression_.cbegin(), expression_.cend()) {
  nextToken();
}

ExprTokenizer& ExprTokenizer::operator=(const ExprTokenizer& t) {
  currentChar_ = t.currentChar_;
  endChar_ = t.endChar_;
  currentToken_ = t.currentToken_;

  return *this;
}

std::ostream& operator<<(std::ostream& os, const ExprTokenizer& t) {
  os << "T{" << std::distance(t.currentChar(), t.endChar()) << "}";
  return os;
}

Result<std::unique_ptr<Expr>> parseExpression(const SymbolTable& symbolTable,
                                              const std::string& expression) {
  return ExprParser(symbolTable, expression).parse();
}

Result<std::unique_ptr<Expr>> parseExpression(const SymbolTable& symbolTable,
                                              const std::u16string& expression) {
  return ExprParser(symbolTable, expression).parse();
}

Result<std::unique_ptr<Expr>> ExprParser::parse() {
  try {
    if (auto e = relExpr(); currentToken_.eof()) {
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
        nextToken();
        lhs = std::make_unique<DefineExpr>(std::move(lhs), addExpr());
        break;
      case Token::Equ:
        nextToken();
        lhs = std::make_unique<EquExpr>(std::move(lhs), addExpr());
        break;
      case Token::Less:
        nextToken();
        lhs = std::make_unique<LessExpr>(std::move(lhs), addExpr());
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
  auto lhs = facExpr();
  for (;;) {
    switch (currentToken()) {
      case Token::Eof:
      case Token::RndClose:
        return lhs;
      case Token::Mul:
        nextToken();
        lhs = std::make_unique<MulExpr>(std::move(lhs), facExpr());
        break;
      case Token::Div:
        nextToken();
        lhs = std::make_unique<DivExpr>(std::move(lhs), facExpr());
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

std::unique_ptr<Expr> ExprParser::facExpr() {
  auto lhs = powExpr();

  for (;;) {
    switch (currentToken()) {
      case Token::Fac:
        nextToken();
        lhs = std::make_unique<FacExpr>(std::move(lhs));
        break;
      default:
        return lhs;
    }
  }
}

std::unique_ptr<Expr> ExprParser::powExpr() {
  auto lhs = primaryExpr();
  while (!currentToken_.eof()) {
    switch (currentToken()) {
      case Token::Pow:
        // a^b^c = a^(b^c)
        nextToken();
        lhs = std::make_unique<PowExpr>(std::move(lhs), powExpr());
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
      return std::make_unique<NumberExpr>(consumeNumber());
    }
    case Token::Symbol: {
      Symbol name = currentToken_->symbol();
      nextToken();

      const Def* def = symbolTable_.lookup(name);
      if (def == nullptr)
        return std::make_unique<SymbolExpr>(name, nullptr);

      if (const ConstantDef* n = dynamic_cast<const ConstantDef*>(def))
        return std::make_unique<SymbolExpr>(name, n);

      if (const FunctionDef* f = dynamic_cast<const FunctionDef*>(def)) {
        // parse ['^' primaryExpr ] ('(' expr (',' expr)* ')'
        //                          |    expr (',' expr)* )
        std::unique_ptr<Expr> power;
        if (tryConsumeToken(Token::Pow))
          power = primaryExpr();

        bool parsedBrackets = tryConsumeToken(Token::RndOpen);
        std::vector<std::unique_ptr<Expr>> inputs;
        inputs.emplace_back(expr());
        while (tryConsumeToken(Token::Comma))
          inputs.emplace_back(expr());

        if (parsedBrackets)
          consumeToken(Token::RndClose);

        // TODO: create call node: {f, inputs}^power
        if (power) {
          return std::make_unique<PowExpr>(
              std::make_unique<CallExpr>(name, f, std::move(inputs)), std::move(power));
        } else {
          return std::make_unique<CallExpr>(name, f, std::move(inputs));
        }
      }

      throw make_error_code(UnexpectedToken);
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

bool ExprParser::tryConsumeToken(Token t) {
  if (currentToken() != t)
    return false;

  nextToken();
  return true;
}

Number ExprParser::consumeNumber() {
  if (currentToken() != Token::Number)
    throw make_error_code(UnexpectedToken);

  Number n = currentToken_->number();
  nextToken();
  return n;
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
    case UnknownSymbol:
      return "Unknown symbol";
  }
}

}  // namespace cmath

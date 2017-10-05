// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <cmath/expr.h>
#include <cmath/result.h>
#include <iterator>
#include <memory>
#include <system_error>
#include <utility>

namespace cmath {

enum class Token {  // {{{
  Eof,
  Number,
  Symbol,
  Equ,
  NotEqu,
  LessEqu,
  GreaterEqu,
  Less,
  Greater,
  Plus,
  Minus,
  Mul,
  Div,
  Pow,
  Fac,
  RndOpen,
  RndClose,
  Define,       // :=
  Equivalence,  // <=>
};              // }}}

std::ostream& operator<<(std::ostream& os, Token t);

class ExprToken {
 public:
  ExprToken() : token_(Token::Eof), number_(), symbol_() {}

  Token token() const noexcept { return token_; }
  Number number() const { return number_; }
  const Symbol& symbol() const { return symbol_; }

  void setToken(Token t) { token_ = t; }

  void setSymbol(const Symbol& s) {
    symbol_ = s;
    setToken(Token::Symbol);
  }

  void setNumber(Number n) {
    number_ = n;
    setToken(Token::Number);
  }

 private:
  Token token_;
  Number number_;
  Symbol symbol_;
};

class ExprTokenizer {
 public:
  explicit ExprTokenizer(const std::u16string& expression);
  ExprTokenizer();

  ExprTokenizer& operator=(const ExprTokenizer& t);

  std::string expression() const;

  bool next();
  bool eof() const;
  size_t offset() const { return std::distance(expression_.begin(), currentChar_); }

  const ExprToken& operator*() const { return currentToken_; }
  const ExprToken* operator->() const { return &currentToken_; }

  ExprTokenizer& operator++() {
    next();
    return *this;
  }

  ExprTokenizer& operator++(int) {
    next();
    return *this;
  }

  bool operator==(const ExprTokenizer& other) const;
  bool operator!=(const ExprTokenizer& other) const;

  friend std::ostream& operator<<(std::ostream& os, const ExprTokenizer& t);

 private:
  std::u16string expression_;
  std::u16string::const_iterator currentChar_;
  ExprToken currentToken_;
};

Result<std::unique_ptr<Expr>> parseExpression(const std::string& expression);
Result<std::unique_ptr<Expr>> parseExpression(const std::u16string& expression);

class ExprParser {
 public:
  explicit ExprParser(const std::string& expression);
  explicit ExprParser(const std::u16string& expression);

  Result<std::unique_ptr<Expr>> parse();

  enum ErrorCode { UnexpectedCharacter, UnexpectedToken, UnexpectedEof };
  class ErrorCategory;

  // ExprTokenizer begin() { return ExprTokenizer(expression_); }
  // ExprTokenizer end() { return ExprTokenizer(); }

 private:
  Token nextToken();
  Token currentToken();
  void consumeToken(Token t);

  std::unique_ptr<Expr> expr();
  std::unique_ptr<Expr> relExpr();      // < > = != <= >=
  std::unique_ptr<Expr> addExpr();      // + -
  std::unique_ptr<Expr> mulExpr();      // * /
  std::unique_ptr<Expr> powExpr();      // ^
  std::unique_ptr<Expr> primaryExpr();  // number symbol ( ! -

 private:
  std::u16string expression_;
  ExprTokenizer currentToken_;
};

class ExprParser::ErrorCategory : public std::error_category {
 public:
  static const ErrorCategory& get();

  const char* name() const noexcept override;
  std::string message(int ec) const override;
};

inline std::error_code make_error_code(ExprParser::ErrorCode ec) {
  return std::error_code(static_cast<int>(ec), ExprParser::ErrorCategory::get());
}

}  // namespace cmath

namespace std {
template <>
struct is_error_code_enum<cmath::ExprParser::ErrorCode> : public true_type {};
}  // namespace std

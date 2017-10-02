// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <cmath/expr.h>
#include <cmath/result.h>
#include <memory>
#include <system_error>
#include <utility>

namespace cmath {

class ExprParser {
 public:
  ExprParser();

  Result<std::unique_ptr<Expr>> parse(const std::string& expression);

  enum ErrorCode { UnexpectedCharacter, UnexpectedToken, UnexpectedEof };
  class ErrorCategory;

 private:
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
  };  // }}}

  bool eof() const { return currentPosition_ == expression_.end(); }
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
  std::string expression_;
  std::string::iterator currentPosition_;
  Number currentNumber_;
  Symbol currentSymbol_;
  Token currentToken_;
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

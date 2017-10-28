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

class SymbolTable;

enum class Token {  // {{{
  Eof,              // <artificial delimiter>
  Number,           // 1234, -5
  Symbol,           // a, b, pi, phi, ...
  Equ,              // =
  NotEqu,           // <>
  LessEqu,          // <=
  GreaterEqu,       // >=
  Less,             // <
  Greater,          // >
  Plus,             // +
  Minus,            // -
  Mul,              // *
  Div,              // /
  Pow,              // ^
  Fac,              // !
  RndOpen,          // (
  RndClose,         // )
  Define,           // :=
  Equivalence,      // <=>
  Comma,            // ,
  Colon,            // :
  RightArrow,       // ->
};                  // }}}

std::ostream& operator<<(std::ostream& os, Token t);

class ExprToken {
 public:
  ExprToken() : token_(Token::Eof), number_(), symbol_() {}

  Token token() const noexcept { return token_; }
  Number number() const { return number_; }
  const Symbol& symbol() const { return symbol_; }

  void setToken(Token t);
  void setSymbol(const Symbol& s);
  void setNumber(Number n);

 private:
  Token token_;
  Number number_;
  Symbol symbol_;
};

class ExprTokenizer {
 public:
  ExprTokenizer(std::u16string::const_iterator begin, std::u16string::const_iterator end);
  ExprTokenizer();

  ExprTokenizer& operator=(const ExprTokenizer& t);

  bool next();
  bool eof() const;

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
  std::u16string::const_iterator currentChar() const { return currentChar_; }
  std::u16string::const_iterator endChar() const { return endChar_; }
  bool hasBytesPending() const { return currentChar() != endChar(); }

 private:
  std::u16string::const_iterator currentChar_;
  std::u16string::const_iterator endChar_;
  ExprToken currentToken_;
};

Result<std::unique_ptr<Expr>> parseExpression(const SymbolTable& st,
                                              const std::string& expression);

Result<std::unique_ptr<Expr>> parseExpression(const SymbolTable& st,
                                              const std::u16string& expression);

class ExprParser {
 public:
  ExprParser(const SymbolTable& symbolTable, const std::string& expression);
  ExprParser(const SymbolTable& symbolTable, const std::u16string& expression);

  Result<std::unique_ptr<Expr>> parse();

  enum ErrorCode { UnexpectedCharacter, UnexpectedToken, UnexpectedEof, UnknownSymbol };
  class ErrorCategory;

  ExprTokenizer begin() { return ExprTokenizer(expression_.begin(), expression_.end()); }
  ExprTokenizer end() { return ExprTokenizer(expression_.cend(), expression_.cend()); }

 private:
  Token nextToken();
  Token currentToken();
  void consumeToken(Token t);
  bool tryConsumeToken(Token t);
  Number consumeNumber();

  std::unique_ptr<Expr> expr();
  std::unique_ptr<Expr> relExpr();      // < > = != <= >=
  std::unique_ptr<Expr> addExpr();      // + -
  std::unique_ptr<Expr> mulExpr();      // * /
  std::unique_ptr<Expr> facExpr();      // !
  std::unique_ptr<Expr> powExpr();      // ^
  std::unique_ptr<Expr> primaryExpr();  // number symbol ( ! -

 private:
  const SymbolTable& symbolTable_;
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

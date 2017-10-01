// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <complex>
#include <iosfwd>
#include <map>
#include <memory>

namespace cmath {

class Expr;

using Number = std::complex<double>;
using Symbol = char;
using SymbolTable = std::map<Symbol, std::unique_ptr<Expr>>;

enum class Precedence {
  Relation,        // < > <= >= != =
  Addition,        // + -
  Multiplication,  // * / !
  Power,           // ^
  Number,          // 42 x a b
};

class Expr {
 public:
  explicit Expr(Precedence p);
  virtual ~Expr() {}

  Precedence precedence() const noexcept { return precedence_; }

  virtual std::string str() const = 0;
  virtual Number calculate(const SymbolTable& t) const = 0;

 protected:
  Precedence precedence_;
};

class NumberExpr : public Expr {
 public:
  explicit NumberExpr(Number n);

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;

 private:
  Number literal_;
};

class NegExpr : public Expr {
 public:
  explicit NegExpr(std::unique_ptr<Expr>&& e);

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;

 private:
  std::unique_ptr<Expr> subExpr_;
};

class SymbolExpr : public Expr {
 public:
  explicit SymbolExpr(Symbol n);

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;

 private:
  Symbol symbol_;
};

class BinaryExpr : public Expr {
 public:
  BinaryExpr(Precedence p,
             char op,
             std::unique_ptr<Expr>&& left,
             std::unique_ptr<Expr>&& right);

  std::string str() const override;

 protected:
  char operator_;
  std::unique_ptr<Expr> left_;
  std::unique_ptr<Expr> right_;
};

class PlusExpr : public BinaryExpr {
 public:
  PlusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
};

class MinusExpr : public BinaryExpr {
 public:
  MinusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
};

class MulExpr : public BinaryExpr {
 public:
  MulExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
};

class DivExpr : public BinaryExpr {
 public:
  DivExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
};

class PowExpr : public BinaryExpr {
 public:
  PowExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
};

std::ostream& operator<<(std::ostream& os, const Expr* expr);
std::ostream& operator<<(std::ostream& os, const Expr& expr);

}  // namespace cmath

// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <assert.h>
#include <cmath/expr.h>
#include <cmath>
#include <iostream>
#include <sstream>

namespace cmath {

// common
std::ostream& operator<<(std::ostream& os, const Expr* expr) {
  return os << expr->str();
}

std::ostream& operator<<(std::ostream& os, const Expr& expr) {
  return os << expr.str();
}

// {{{ Expr
Expr::Expr(Precedence p) : precedence_(p) {}
// }}}
// {{{ NumberExpr
NumberExpr::NumberExpr(Number n) : Expr(Precedence::Number), literal_(n) {}

std::string NumberExpr::str() const {
  std::stringstream s;
  if (literal_.imag()) {
    s << literal_.real() << '+' << literal_.imag() << 'i';
  } else {
    s << literal_.real();
  }
  return s.str();
}

Number NumberExpr::calculate(const SymbolTable& /*t*/) const {
  return literal_;
}
// }}}
// {{{ NegExpr
NegExpr::NegExpr(std::unique_ptr<Expr>&& e)
    : Expr(Precedence::Number), subExpr_(std::move(e)) {}

std::string NegExpr::str() const {
  std::stringstream s;

  s << '-';

  if (subExpr_->precedence() < precedence()) {
    s << '(' << subExpr_->str() << ')';
  } else {
    s << subExpr_->str();
  }

  return s.str();
}  // namespace cmath

Number NegExpr::calculate(const SymbolTable& t) const {
  return -subExpr_->calculate(t);
}
// }}}
// {{{ SymbolExpr
SymbolExpr::SymbolExpr(Symbol s) : Expr(Precedence::Number), symbol_(s) {}

std::string SymbolExpr::str() const {
  std::stringstream s;
  s << symbol_;
  return s.str();
}

Number SymbolExpr::calculate(const SymbolTable& t) const {
  assert(t.find(symbol_) != t.end());
  auto i = t.find(symbol_);
  if (i != t.end())
    return i->second->calculate(t);
  else
    return 0;
}
// }}}
// {{{ BinaryExpr
BinaryExpr::BinaryExpr(Precedence p,
                       char op,
                       std::unique_ptr<Expr>&& left,
                       std::unique_ptr<Expr>&& right)
    : Expr(p), operator_(op), left_(std::move(left)), right_(std::move(right)) {}

std::string BinaryExpr::str() const {
  std::stringstream s;

  if (left_->precedence() < precedence())
    s << '(' << *left_ << ')';
  else
    s << *left_;

  s << operator_;

  if (right_->precedence() < precedence())
    s << '(' << *right_ << ')';
  else
    s << *right_;

  return s.str();
}
// }}}
// {{{ PlusExpr
PlusExpr::PlusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Addition, '+', std::move(left), std::move(right)) {}

Number PlusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) + right_->calculate(t);
}
// }}}
// {{{ MinusExpr
MinusExpr::MinusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Addition, '-', std::move(left), std::move(right)) {}

Number MinusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) - right_->calculate(t);
}
// }}}
// {{{ MulExpr
MulExpr::MulExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Multiplication, '*', std::move(left), std::move(right)) {}

Number MulExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) * right_->calculate(t);
}
// }}}
// {{{ DivExpr
DivExpr::DivExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Multiplication, '/', std::move(left), std::move(right)) {}

Number DivExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) / right_->calculate(t);
}
// }}}
// {{{ PowExpr
PowExpr::PowExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Multiplication, '^', std::move(left), std::move(right)) {}

Number PowExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);
  return std::pow(a, b);
}
// }}}

}  // namespace cmath

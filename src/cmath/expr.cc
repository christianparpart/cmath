// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <iostream>
#include <sstream>
#include <cmath>

namespace cmath {

// common
std::ostream& operator<<(std::ostream& os, const Expr* expr) {
  return os << expr->str();
}

std::ostream& operator<<(std::ostream& os, const Expr& expr) {
  return os << expr.str();
}

// {{{ Expr
Expr::Expr(Precedence p) : precedence_(p) {
}
// }}}
// {{{ NumberExpr
NumberExpr::NumberExpr(Number n)
    : Expr(Precedence::Number), literal_(n) {
}

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
// {{{ BinaryExpr
BinaryExpr::BinaryExpr(Precedence p, Expr* left, Expr* right)
    : Expr(p), left_(left), right_(right) {
}
// }}}
// {{{ PlusExpr
PlusExpr::PlusExpr(Expr* left, Expr* right)
    : BinaryExpr(Precedence::Addition, left, right) {
}

std::string PlusExpr::str() const {
  std::stringstream s;
  s << *left_ << "+" << *right_;
  return s.str();
}

Number PlusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) + right_->calculate(t);
}
// }}}
// {{{ MinusExpr
MinusExpr::MinusExpr(Expr* left, Expr* right)
    : BinaryExpr(Precedence::Addition, left, right) {
}

std::string MinusExpr::str() const {
  std::stringstream s;
  s << *left_ << "-" << *right_;
  return s.str();
}

Number MinusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) - right_->calculate(t);
}
// }}}
// {{{ MulExpr
MulExpr::MulExpr(Expr* left, Expr* right)
    : BinaryExpr(Precedence::Multiplication, left, right) {
}

std::string MulExpr::str() const {
  std::stringstream s;
  s << *left_ << "+" << *right_;
  return s.str();
}

Number MulExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) * right_->calculate(t);
}
// }}}
// {{{ DivExpr
DivExpr::DivExpr(Expr* left, Expr* right)
    : BinaryExpr(Precedence::Multiplication, left, right) {
}

std::string DivExpr::str() const {
  std::stringstream s;
  s << *left_ << "+" << *right_;
  return s.str();
}

Number DivExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) / right_->calculate(t);
}
// }}}
// {{{ PowExpr
PowExpr::PowExpr(Expr* left, Expr* right)
    : BinaryExpr(Precedence::Multiplication, left, right) {
}

std::string PowExpr::str() const {
  std::stringstream s;
  s << *left_ << "+" << *right_;
  return s.str();
}

Number PowExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);
  return std::pow(a, b);
}
// }}}

} // namespace cmath

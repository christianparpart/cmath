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

  if (!literal_.imag()) {
    s << literal_.real();
  } else {
    if (literal_.real()) {
      s << literal_.real() << " + ";
    }
    if (literal_.imag() != 1) {
      s << literal_.imag();
    }
    s << 'i';
  }
  return s.str();
}

Number NumberExpr::calculate(const SymbolTable& /*t*/) const {
  return literal_;
}

std::unique_ptr<Expr> NumberExpr::clone() const {
  return std::make_unique<NumberExpr>(literal_);
}

bool NumberExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const NumberExpr*>(other))
    return e->literal_ == literal_;

  return false;
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

std::unique_ptr<Expr> NegExpr::clone() const {
  return std::make_unique<NegExpr>(subExpr_->clone());
}

bool NegExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const NegExpr*>(other))
    return e->compare(subExpr_.get());

  return false;
}
// }}}
// {{{ SymbolExpr
SymbolExpr::SymbolExpr(const Symbol& s) : Expr(Precedence::Number), symbol_(s) {}

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

std::unique_ptr<Expr> SymbolExpr::clone() const {
  return std::make_unique<SymbolExpr>(symbol_);
}

bool SymbolExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const SymbolExpr*>(other))
    return e->symbol_ == symbol_;

  return false;
}
// }}}
// {{{ BinaryExpr
BinaryExpr::BinaryExpr(Precedence p,
                       const std::string& op,
                       std::unique_ptr<Expr>&& left,
                       std::unique_ptr<Expr>&& right)
    : Expr(p), operator_(op), left_(std::move(left)), right_(std::move(right)) {}

std::string BinaryExpr::str() const {
  std::stringstream s;

  if (left_->precedence() < precedence())
    s << '(' << *left_ << ')';
  else
    s << *left_;

  s << ' ' << operator_ << ' ';

  if (right_->precedence() < precedence())
    s << '(' << *right_ << ')';
  else
    s << *right_;

  return s.str();
}
// }}}
// {{{ PlusExpr
PlusExpr::PlusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Addition, "+", std::move(left), std::move(right)) {}

Number PlusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) + right_->calculate(t);
}

std::unique_ptr<Expr> PlusExpr::clone() const {
  return std::make_unique<PlusExpr>(left_->clone(), right_->clone());
}

bool PlusExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const PlusExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ MinusExpr
MinusExpr::MinusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Addition, "-", std::move(left), std::move(right)) {}

Number MinusExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) - right_->calculate(t);
}

std::unique_ptr<Expr> MinusExpr::clone() const {
  return std::make_unique<MinusExpr>(left_->clone(), right_->clone());
}

bool MinusExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const MinusExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ MulExpr
MulExpr::MulExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Multiplication, "*", std::move(left), std::move(right)) {}

Number MulExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) * right_->calculate(t);
}

std::unique_ptr<Expr> MulExpr::clone() const {
  return std::make_unique<MulExpr>(left_->clone(), right_->clone());
}

bool MulExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const MulExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ DivExpr
DivExpr::DivExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Multiplication, "/", std::move(left), std::move(right)) {}

Number DivExpr::calculate(const SymbolTable& t) const {
  return left_->calculate(t) / right_->calculate(t);
}

std::unique_ptr<Expr> DivExpr::clone() const {
  return std::make_unique<DivExpr>(left_->clone(), right_->clone());
}

bool DivExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const DivExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ FacExpr
FacExpr::FacExpr(std::unique_ptr<Expr>&& subExpr)
    : UnaryExpr(Precedence::Number, std::move(subExpr)) {
}

std::string FacExpr::str() const {
  std::stringstream s;
  if (subExpr()->precedence() < precedence())
    s << '(' << subExpr()->str() << ')';
  else
    s << subExpr()->str();

  s << '!';

  return s.str();
}

Number FacExpr::calculate(const SymbolTable& t) const {
  Number y = 1;
  Number i = 1;
  Number n = subExpr()->calculate(t); 

  while (i.real() <= n.real()) {
    y *= i;
    i += 1;
  }

  return y;
}

std::unique_ptr<Expr> FacExpr::clone() const {
  return std::make_unique<FacExpr>(subExpr()->clone());
}

bool FacExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const FacExpr*>(other))
    return e->subExpr()->compare(subExpr());

  return false;
}
// }}}
// {{{ PowExpr
PowExpr::PowExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Power, "^", std::move(left), std::move(right)) {}

Number PowExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);
  return std::pow(a, b);
}

std::unique_ptr<Expr> PowExpr::clone() const {
  return std::make_unique<PowExpr>(left_->clone(), right_->clone());
}

bool PowExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const PowExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ EquExpr
EquExpr::EquExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Relation, "=", std::move(left), std::move(right)) {}

Number EquExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);
  return a == b ? 1 : 0;
}

std::unique_ptr<Expr> EquExpr::clone() const {
  return std::make_unique<EquExpr>(left_->clone(), right_->clone());
}

bool EquExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const EquExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ LessExpr
LessExpr::LessExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Relation, "<", std::move(left), std::move(right)) {}

Number LessExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);

  if (!a.imag() && !b.imag())
    return a.real() < b.real() ? 1 : 0;
  else
    return 0;
}

std::unique_ptr<Expr> LessExpr::clone() const {
  return std::make_unique<LessExpr>(left_->clone(), right_->clone());
}

bool LessExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const LessExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}
// }}}
// {{{ DefineExpr
DefineExpr::DefineExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right)
    : BinaryExpr(Precedence::Relation, ":=", std::move(left), std::move(right)) {
  if (!dynamic_cast<SymbolExpr*>(this->left())) {
    throw "DefineExpr: no symbol found on left-hand side, but expects one";
  }
}

Number DefineExpr::calculate(const SymbolTable& t) const {
  Number a = left_->calculate(t);
  Number b = right_->calculate(t);
  return a == b ? 1 : 0;
}

std::unique_ptr<Expr> DefineExpr::clone() const {
  return std::make_unique<DefineExpr>(left_->clone(), right_->clone());
}

bool DefineExpr::compare(const Expr* other) const {
  if (auto e = dynamic_cast<const DefineExpr*>(other))
    return e->left_->compare(left_.get()) && e->right_->compare(right_.get());

  return false;
}

const Symbol& DefineExpr::symbolName() const {
  if (auto e = dynamic_cast<const SymbolExpr*>(left()))
    return e->symbolName();

  throw "DefineExpr: no symbol found on left-hand side, but expects one";
}
// }}}

}  // namespace cmath

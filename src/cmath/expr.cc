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
NumberExpr::NumberExpr(Number n) : Expr(Precedence::Primary), literal_(n) {}

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
    : Expr(Precedence::Primary), subExpr_(std::move(e)) {}

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
SymbolExpr::SymbolExpr(const Symbol& s, const ConstantDef* def)
    : Expr(Precedence::Primary), symbol_(s), def_(def) {}

std::string SymbolExpr::str() const {
  std::stringstream s;
  s << symbol_;
  return s.str();
}

Number SymbolExpr::calculate(const SymbolTable& t) const {
  if (def_)
    return def_->getNumber();

  if (const Def* d = t.lookup(symbol_)) {
    if (auto i = dynamic_cast<const ConstantDef*>(d)) {
      const_cast<SymbolExpr*>(this)->def_ = i;
      return i->getNumber();
    }
  }

  return std::nan("");
}

std::unique_ptr<Expr> SymbolExpr::clone() const {
  return std::make_unique<SymbolExpr>(symbol_, def_);
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
    : UnaryExpr(Precedence::Primary, std::move(subExpr)) {}

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
  if (!a.imag() && a.real() == M_E) {
    return std::exp(b);
  } else {
    return std::pow(a, b);
  }
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
  if (a == b)
    return a;
  else
    return Number(std::nan(""), std::nan(""));
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

  if (!a.imag() && !b.imag() && a.real() < b.real())
    return a;
  else
    return Number(std::nan(""), std::nan(""));
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
// {{{ SymbolTable
SymbolTable::SymbolTable() : symbols_(), outerScope_(nullptr) {}

SymbolTable::SymbolTable(const SymbolTable* outerScope)
    : symbols_(), outerScope_(outerScope) {}

void SymbolTable::defineConstant(const Symbol& name, Number value) {
  symbols_[name] = std::make_unique<ConstantDef>(value);
}

void SymbolTable::defineFunction(const Symbol& name, NativeFunctionDef::Impl impl) {
  symbols_[name] = std::make_unique<NativeFunctionDef>(impl);
}

void SymbolTable::defineFunction(const Symbol& name,
                                 const CustomFunctionDef::SymbolList& inputs,
                                 std::unique_ptr<Expr>&& impl) {
  symbols_[name] = std::make_unique<CustomFunctionDef>(inputs, std::move(impl));
}

const Def* SymbolTable::lookup(const Symbol& name) const {
  auto i = symbols_.find(name);
  if (i != symbols_.end()) {
    return i->second.get();
  } else if (outerScope_) {
    return outerScope_->lookup(name);
  } else {
    return nullptr;
  }
}

Number SymbolTable::getNumber(const Symbol& name) const {
  if (const Def* d = lookup(name))
    if (auto n = dynamic_cast<const ConstantDef*>(d))
      return n->getNumber();

  return std::nan("");
}
// }}}
// {{{ CallExpr
CallExpr::CallExpr(const std::string& name, const FunctionDef* f, ParamList&& inputs)
    : Expr(Precedence::Primary), symbolName_(name), function_(f),
      inputs_(std::move(inputs)) {}

Number CallExpr::calculate(const SymbolTable& t) const {
  FunctionDef::NumberList args;
  args.resize(inputs_.size());
  for (size_t i = 0, e = inputs_.size(); i != e; ++i)
    args[i] = inputs_[i]->calculate(t);

  return function_->call(t, args);
}

std::string CallExpr::str() const {
  std::stringstream s;
  s << symbolName_ << '(';
  for (size_t i = 0, e = inputs_.size(); i != e; ++i) {
    if (i)
      s << ", ";
    s << inputs_[i]->str();
  }
  s << ')';
  return s.str();
}

std::unique_ptr<Expr> CallExpr::clone() const {
  ParamList args;
  args.resize(inputs_.size());
  for (int i = 0, e = inputs_.size(); i != e; ++i)
    args[i] = inputs_[i]->clone();

  return std::make_unique<CallExpr>(symbolName_, function_, std::move(args));
}

bool CallExpr::compare(const Expr* other) const {
  if (auto otherCall = dynamic_cast<const CallExpr*>(other)) {
    if (otherCall->function_ == function_) {
      for (int i = 0, e = inputs_.size(); i != e; ++i) {
        if (!inputs_[i]->compare(otherCall->inputs_[i].get())) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}
// }}}
// {{{ ConstantDef
ConstantDef::ConstantDef(Number value) : number_(value) {}

std::string ConstantDef::str() const {
  std::stringstream s;
  if (!number_.imag()) {
    s << number_.real();
  } else {
    s << "(" << number_.real() << " + ";
    if (number_.imag() != 1) {
      s << number_.imag();
    }
    s << "i)";
  }
  return s.str();
}
// }}}
// {{{ NativeFunctionDef
NativeFunctionDef::NativeFunctionDef(Impl impl) : impl_(impl) {}

Number NativeFunctionDef::call(const SymbolTable& t, const NumberList& input) const {
  return impl_(input[0]);
}

std::string NativeFunctionDef::str() const {
  return "(x) -> native";
}
// }}}
// {{{ CustomFunctionDef
CustomFunctionDef::CustomFunctionDef(const SymbolList& inputs,
                                     std::unique_ptr<Expr>&& expr)
    : inputs_(inputs), expr_(std::move(expr)) {}

Number CustomFunctionDef::call(const SymbolTable& t, const NumberList& inputs) const {
  SymbolTable st(&t);

  for (size_t i = 0, e = inputs_.size(); i != e; ++i)
    st.defineConstant(inputs_[i], inputs[i]);

  return expr_->calculate(t);
}

std::string CustomFunctionDef::str() const {
  std::stringstream s;

  s << "(";
  for (size_t i = 0, e = inputs_.size(); i != e; ++i) {
    if (i)
      s << ", ";
    s << inputs_[i];
  }
  s << ") = " << expr_->str();

  return s.str();
}
// }}}

}  // namespace cmath

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
#include <string>
#include <vector>

namespace cmath {

class SymbolTable;

using Number = std::complex<double>;
using Symbol = std::string;

enum class Precedence {
  Relation,        // < > <= >= != =
  Addition,        // + -
  Multiplication,  // * / !
  Power,           // ^
  Primary,         // 42 x a b
};

// TODO: rename calculate to evaluate() and make it more generic
// TODO: -> provide a toBool(const Expr*)
// TODO: -> provide a toNumber(const Expr*) equivalent

class Expr {
 public:
  explicit Expr(Precedence p);
  virtual ~Expr() {}

  Precedence precedence() const noexcept { return precedence_; }

  virtual std::string str() const = 0;
  virtual Number calculate(const SymbolTable& t) const = 0;
  virtual std::unique_ptr<Expr> clone() const = 0;
  virtual bool compare(const Expr* other) const = 0;

 protected:
  Precedence precedence_;
};

class NumberExpr : public Expr {
 public:
  explicit NumberExpr(Number n);

  Number getNumber() const { return literal_; }

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;

 private:
  Number literal_;
};

class UnaryExpr : public Expr {
 public:
  UnaryExpr(Precedence precedence, std::unique_ptr<Expr>&& subExpr)
      : Expr(precedence), subExpr_(std::move(subExpr)) {}

  const Expr* subExpr() const { return subExpr_.get(); }

 protected:
  std::unique_ptr<Expr> subExpr_;
};

class FacExpr : public UnaryExpr {
 public:
  explicit FacExpr(std::unique_ptr<Expr>&& subExpr);

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class NegExpr : public Expr {
 public:
  explicit NegExpr(std::unique_ptr<Expr>&& e);

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;

 private:
  std::unique_ptr<Expr> subExpr_;
};

class ConstantDef;
class SymbolExpr : public Expr {
 public:
  SymbolExpr(const Symbol& n, const ConstantDef* def);

  const Symbol& symbolName() const noexcept { return symbol_; }

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;

 private:
  Symbol symbol_;
  const ConstantDef* def_;
};

class BinaryExpr : public Expr {
 public:
  BinaryExpr(Precedence p,
             const std::string& op,
             std::unique_ptr<Expr>&& left,
             std::unique_ptr<Expr>&& right);

  std::string str() const override;

  Expr* left() const { return left_.get(); }
  Expr* right() const { return right_.get(); }

 protected:
  std::string operator_;
  std::unique_ptr<Expr> left_;
  std::unique_ptr<Expr> right_;
};

class PlusExpr : public BinaryExpr {
 public:
  PlusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class MinusExpr : public BinaryExpr {
 public:
  MinusExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class MulExpr : public BinaryExpr {
 public:
  MulExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class DivExpr : public BinaryExpr {
 public:
  DivExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class PowExpr : public BinaryExpr {
 public:
  PowExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class EquExpr : public BinaryExpr {
 public:
  EquExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class LessExpr : public BinaryExpr {
 public:
  LessExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class DefineExpr : public BinaryExpr {
 public:
  // definiendum := definiens
  DefineExpr(std::unique_ptr<Expr>&& left, std::unique_ptr<Expr>&& right);

  const Symbol& symbolName() const;

  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;
};

class FunctionDef;

class CallExpr : public Expr {
 public:
  using ParamList = std::vector<std::unique_ptr<Expr>>;

  CallExpr(const std::string& symbolName, const FunctionDef* f, ParamList&& inputs);

  Number calculate(const SymbolTable& t) const override;
  std::string str() const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;

 private:
  std::string symbolName_;
  const FunctionDef* function_;
  ParamList inputs_;
};

class Def {
 public:
  virtual ~Def() {}

  virtual std::string str() const = 0;
};

class ConstantDef : public Def {
 public:
  explicit ConstantDef(Number value);

  Number getNumber() const { return number_; }

  void redefine(Number n) { number_ = n; }

  std::string str() const override;

 private:
  Number number_;
};

class FunctionDef : public Def {
 public:
  using NumberList = std::vector<Number>;

  virtual Number call(const SymbolTable& t, const NumberList& inputs) const = 0;
};

class NativeFunctionDefEx : public FunctionDef {
 public:
  using result_type = std::unique_ptr<Expr>;
  using Impl = std::function<result_type(const Expr*)>;

  explicit NativeFunctionDef(Impl impl);

  Number call(const SymbolTable& t, std::vector<const Expr*>& args) const;
  std::string str() const override;

 private:
  Impl impl_;
};

class NativeFunctionDef : public FunctionDef {
 public:
  using Impl = std::function<Number(Number)>;

  explicit NativeFunctionDef(Impl impl);

  Number call(const SymbolTable& t, const NumberList& inputs) const override;
  std::string str() const override;

 private:
  Impl impl_;
};

class NativeFunction2Def : public FunctionDef {
 public:
  using Impl = std::function<Number(Number, Number)>;

  explicit NativeFunction2Def(Impl impl);

  Number call(const SymbolTable& t, const NumberList& inputs) const override;
  std::string str() const override;

 private:
  Impl impl_;
};

class CustomFunctionDef : public FunctionDef {
 public:
  using SymbolList = std::vector<Symbol>;

  CustomFunctionDef(const SymbolList& inputs, std::unique_ptr<Expr>&& expression);

  Number call(const SymbolTable& t, const NumberList& inputs) const override;
  std::string str() const override;

 private:
  SymbolList inputs_;
  std::unique_ptr<Expr> expr_;
};

class SymbolTable {
 public:
  SymbolTable();
  explicit SymbolTable(const SymbolTable* outerScope);

  void defineConstant(const Symbol& name, Number value);
  void defineFunction(const Symbol& name, NativeFunctionDef::Impl impl);
  void defineFunction(const Symbol& name, NativeFunction2Def::Impl impl);
  void defineFunction(const Symbol& name,
                      const CustomFunctionDef::SymbolList& inputs,
                      std::unique_ptr<Expr>&& impl);

  void undefine(const Symbol& name);

  const Def* lookup(const Symbol& name) const;

  using Map = std::map<Symbol, std::unique_ptr<Def>>;
  using iterator = Map::iterator;
  using const_iterator = Map::const_iterator;

  iterator begin() { return symbols_.begin(); }
  iterator end() { return symbols_.end(); }

  const_iterator begin() const { return symbols_.cbegin(); }
  const_iterator end() const { return symbols_.cend(); }

  const_iterator cbegin() const { return symbols_.cbegin(); }
  const_iterator cend() const { return symbols_.cend(); }

 private:
  Map symbols_;
  const SymbolTable* outerScope_;
};

class CaseExpr : public Expr {
 public:
  using CaseMatch = std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>;
  using CaseList = std::vector<CaseMatch>;

  CaseExpr(CaseList&& cases, std::unique_ptr<Expr>&& elseExpr);
  CaseExpr(std::unique_ptr<Expr>&& condExpr,
           std::unique_ptr<Expr>&& trueExpr,
           std::unique_ptr<Expr>&& elseExpr);

  const CaseList& cases() const { return cases_; }
  const Expr* elseExpr() const { return elseExpr_.get(); }

  std::string str() const override;
  Number calculate(const SymbolTable& t) const override;
  std::unique_ptr<Expr> clone() const override;
  bool compare(const Expr* other) const override;

 private:
  CaseList cases_;
  std::unique_ptr<Expr> elseExpr_;
};

class CompoundExpr : public Expr {
 public:
  using ExprList = std::vector<std::unique_ptr<Expr>>;

  explicit CompoundExpr(ExprList&& expressions);

  const ExprList& expressions() const { return expressions_; }

 private:
  ExprList expressions_;
};

std::ostream& operator<<(std::ostream& os, const Expr* expr);
std::ostream& operator<<(std::ostream& os, const Expr& expr);

}  // namespace cmath

// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <iostream>

using namespace cmath;

int main(int argc, const char* argv[]) {
  std::unique_ptr<Expr> e(new PlusExpr(new NumberExpr(42), new NumberExpr(3)));
  SymbolTable symbols;

  std::cout << "Expression : " << e->str() << '\n';
  std::cout << "Result     : " << e->calculate(symbols) << '\n';
  return 0;
}

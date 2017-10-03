// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <cmath/expr_parser.h>
#include <iostream>

using namespace cmath;

int main(int argc, const char* argv[]) {
  try {
    SymbolTable symbols;

    symbols['x'] = std::make_unique<NumberExpr>(2);
    symbols['a'] = std::make_unique<MulExpr>(
        std::make_unique<SymbolExpr>('x'),
        std::make_unique<PlusExpr>(std::make_unique<NumberExpr>(3),
                                   std::make_unique<NumberExpr>(1)));

    for (const auto& e : symbols)
      std::cout << e.first << " = " << e.second->str() << std::endl;

    const auto& a = symbols['a'];
    std::cout << "Expression : " << a->str() << '\n';
    std::cout << "Result     : " << a->calculate(symbols) << '\n';

    if (argc > 1) {
      Result<std::unique_ptr<Expr>> e = ExprParser().parse(argv[1]);
      if (!e) {
        std::cout << e.error().category().name() << ": " << e.error().message() << '\n';
        return 1;
      }
      std::cout << "Expression : " << (*e)->str() << '\n';
    }

    return 0;
  } catch (std::error_code ec) {
    std::cout << ec.category().name() << ": " << ec.message() << '\n';
    return 1;
  }
}

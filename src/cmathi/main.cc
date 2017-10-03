// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <cmath/expr.h>
#include <cmath/expr_parser.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace cmath;

std::string simple(Number n) {
  std::stringstream s;
  if (!n.imag()) {
    s << n.real();
  } else {
    if (n.real()) {
      s << n.real() << " + ";
    }
    if (n.imag() != 1) {
      s << n.imag();
    }
    s << 'i';
  }
  return s.str();
}

void declareStandardSymbols(SymbolTable* st) {
  (*st)["i"] = std::make_unique<NumberExpr>(Number(0, 1));
  (*st)["e"] = std::make_unique<NumberExpr>(Number(M_E));
  (*st)[u8"π"] = std::make_unique<NumberExpr>(Number(M_PI));
}

int main(int argc, const char* argv[]) {
  try {
    SymbolTable symbols;
    declareStandardSymbols(&symbols);

    for (const auto& e : symbols)
      std::cout << e.first << " = " << e.second->str() << std::endl;

    if (argc > 1) {
      Result<std::unique_ptr<Expr>> e = ExprParser().parse(argv[1]);
      if (!e) {
        std::cout << e.error().category().name() << ": " << e.error().message() << '\n';
        return 1;
      }
      std::cout << "Expression : " << (*e)->str() << '\n';
      std::cout << "Result     : " << simple((*e)->calculate(symbols)) << '\n';
    }

    return 0;
  } catch (std::error_code ec) {
    std::cerr << ec.category().name() << ": " << ec.message() << '\n';
    return 1;
  } catch (const char* msg) {
    std::cerr << "Error. " << msg << '\n';
    return 1;
  }
}

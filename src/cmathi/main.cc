// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include "console.h"
#include <cmath/expr.h>
#include <cmath/expr_parser.h>
#include <iomanip>
#include <iostream>
#include <sstream>

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

void dumpSymbols(const SymbolTable& symbols) {
  for (const auto& e : symbols)
    std::cout << e.first << " = " << e.second->str() << std::endl;
}

void calculate(const std::string expression, const SymbolTable& st) {
  Result<std::unique_ptr<Expr>> e = parseExpression(expression);
  if (!e)
    throw e;

  std::cout << (*e)->str() << " = " << simple((*e)->calculate(st)) << '\n';
}

int main(int argc, const char* argv[]) {
  try {
    SymbolTable symbols;
    declareStandardSymbols(&symbols);
    dumpSymbols(symbols);
    Readline input(".cmathirc");;
    input.addHistory(u8"e^(i*π) + 1");

    for (;;) {
      const char* line = input.getline(": ");
      if (line == nullptr) // EOF
        return 0;
      else if (*line != '\0') {
        calculate(line, symbols);
      }
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

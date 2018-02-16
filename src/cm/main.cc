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

void injectStandardSymbols(SymbolTable* st) {
  st->defineConstant("i", {0, 1});
  st->defineConstant("e", M_E);
  st->defineConstant("pi", std::acos(-1));
  st->defineConstant(u8"π", std::acos(-1));
  st->defineConstant("nan", std::nan(""));

  st->defineFunction("Re", [](Number x) { return x.real(); });
  st->defineFunction("Im", [](Number x) { return x.imag(); });
  st->defineFunction("arg", [](Number x) { return std::arg(x); });
  st->defineFunction("sin", [](Number x) { return std::sin(x); });
  st->defineFunction("cos", [](Number x) { return std::cos(x); });
  st->defineFunction("tan", [](Number x) { return std::tan(x); });
  st->defineFunction("exp", [](Number x) { return std::exp(x); });
  st->defineFunction("sqrt", [](Number x) { return std::sqrt(x); });
  st->defineFunction("log", [](Number x) { return std::log(x); });

  st->defineFunction("polar", [](Number a, Number b) { return std::polar(a.real(), b.real()); });
}

void dumpSymbols(const SymbolTable& symbolTable) {
  for (const auto& e : symbolTable)
    std::cout << e.first << " = " << e.second->str() << std::endl;
}

void printCommands() {
  std::cout << "Valid input:\n"
            << "?             prints this help\n"
            << "vars          prints all defined variables\n"
            << "EXPR          evaluates given expression\n"
            << "SYM := EXPR   defines a new constant by given expression, e.g. a := 3\n"
            << "quit          Exists program\n";
}

int main(int argc, const char* argv[]) {
  try {
    SymbolTable symbolTable;
    injectStandardSymbols(&symbolTable);
    Readline input(".cmathirc");
    input.addHistory(u8"e^(i*π) + 1");

    std::cout << "Type ? for help.\n";

    for (;;) {
      std::string line;
      bool eof = input.getline(": ", &line);
      if (eof || line == "quit") {
        std::cout << "Bye." << std::endl;
        return 0;
      }

      if (line.empty())
        continue;

      if (line == "vars") {
        dumpSymbols(symbolTable);
        continue;
      }

      if (line == "?") {
        printCommands();
        continue;
      }

      Result<std::unique_ptr<Expr>> e = parseExpression(symbolTable, line);
      if (e.error()) {
        std::error_code ec = e.error();
        std::cerr << ec.category().name() << ": " << ec.message() << '\n';
      } else if (const auto d = dynamic_cast<DefineExpr*>(e->get())) {
        Number value = d->right()->calculate(symbolTable);
        // check NAN
        if (!std::isnan(std::abs(value))) {
          std::cout << "define " << d->str() << '\n';
          symbolTable.defineConstant(d->symbolName(), value);
        } else {
          std::cout << "undefine " << d->str() << '\n';
          symbolTable.undefine(d->symbolName());
        }
      } else {
        std::cout << (*e)->str() << " = " << simple((*e)->calculate(symbolTable)) << '\n';
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

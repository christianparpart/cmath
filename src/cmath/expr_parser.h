// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

#include <cmath/expr.h>

namespace cmath {

class ExprParser {
 public:
  ExprParser(SymbolTable& st);
  std::unique_ptr<Expr> parse(const std::string& expression);
};

} // namespace cmath

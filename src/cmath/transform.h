// This file is part of the "cmath" project, http://github.com/christianparpart/cmath>
//   (c) 2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#pragma once

namespace cmath {

// a^2 + 2ab + b^2 -> (a + b)^n
// a + a = 2a
// a + n*a = (n + 1)*a
std::unique_ptr<Expr> factor(const Expr* e);

// (a + b)^n -> a^2 + 2ab + b^2
std::unique_ptr<Expr> expand(const Expr* e);

std::unique_ptr<Expr> simplify(const Expr* e);

} // namespace cmath

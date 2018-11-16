#pragma once

#include <vector>

#if 0
class Ratio {
 public:
 private:
  Integer nominator_;
  Natural denominator_;
};

class Real {
 public:
  Real(long long nominator, unsigned long long denominator);
  explicit Real(long long v);

  Real(const Real& v);
  Real(Real&& v);

  Real& operator=(const Real& v);
  Real& operator=(Real&& v);

  std::ostream& operator<<(std::ostream& os);

 private:
  std::vector<uint8_t> digits_;
  std::vector<uint8_t> decimals_; // fraction
};

Real operator+(Real a, Real b);
Real operator-(Real a, Real b);
Real operator*(Real a, Real b);
Real operator/(Real a, Real b);

#endif

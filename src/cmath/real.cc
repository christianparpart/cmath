#include <cmath/real.h>

Real::Real(long long nominator, unsigned long long denominator) {
  // TODO
}

Real::Real(long long v) {
  // count decimal positions
  unsigned decimalCount = 0;
  for (long long i = v; i != 0; i /= 10)
    decimalCount++;

  digits_.reserve(decimalCount_);

  for (long long i = v; i != 0; i /= 10)
    digits_.push_front(i % 10);
}

Real::Real(const Real& v)
    : digits_(v.digits_),
      decimals_(v.decimals_) {
}

Real::Real(Real&& v)
    : digits_(std::move(v.digits_)),
      decimals_(std::move(v.decimals_)) {
}

Real& Real::operator=(const Real& v) {
  digits_ = v.digits_;
  decimals_ = v.decimals_;
  return *this;
}

Real& Real::operator=(Real&& v) {
  digits_ = std::move(v.digits_);
  decimals_ = std::move(v.decimals_);
  return *this;
}

std::ostream& Real::operator<<(std::ostream& os) {
  for (uint8_t digit: digits_)
    os << (char)(digit + '0');

  os << '.';

  for (uint8_t digit: decimals_)
    os << (char)(digit + '0');

  return *this;
}


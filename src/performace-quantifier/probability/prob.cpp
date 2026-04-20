#include "prob.hpp"


prob prob::operator+(const prob &other) const {
    return prob(val + other.val);
}

prob &prob::operator+=(const prob &other) {
    val += other.val;
    return *this;
}

prob &prob::operator-=(const prob other) {
    val -= other.val;
    return *this;
}

prob &prob::operator*=(const prob other) {
    val *= other.val;
    return *this;
}

prob &prob::operator/=(const prob other) {
    if (other.val == 0.0)
        throw std::runtime_error("Division by zero in prob");
    val /= other.val;
    return *this;
}

prob prob::operator-(const prob &other) const {
    return prob(val - other.val);
}

prob prob::operator*(const prob &other) const {
    return prob(val * other.val);
}

prob prob::operator/(const prob &other) const {
    if (other.val == 0.0) throw std::runtime_error("Division by zero");
    return prob(val / other.val);
}

bool prob::operator==(const prob &other) const {
    return val == other.val;
}

bool prob::operator!=(const prob other) const { return val != other.val; }

bool prob::operator<(const prob &other) const {
    return val < other.val;
}

bool prob::operator>(const prob &other) const {
    return val > other.val;
}

bool prob::operator<=(const prob &other) const { return val <= other.val; }

bool prob::operator>=(const prob &other) const { return val >= other.val; }

prob::prob(const double val) : val(val) {
}

prob::operator double() const {
    return val;
}

prob min(const prob &a, const prob &b) {
    return prob(std::min(a.val, b.val));
}

prob max(const prob &a, const prob &b) {
    return prob(std::max(a.val, b.val));
}

std::ostream &operator<<(std::ostream &os, const prob &val) {
    return os << val.val;
}

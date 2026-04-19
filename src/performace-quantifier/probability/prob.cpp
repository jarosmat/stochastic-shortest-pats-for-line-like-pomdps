#include "prob.hpp"

#include <stdexcept>

prob prob::operator+(const prob &other) const {
    return prob(val + other.val);
}

prob & prob::operator+=(const prob &other) {
    val += other.val;
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

bool prob::operator<(const prob &other) const {
    return val < other.val;
}

bool prob::operator>(const prob &other) const {
    return val > other.val;
}

prob::prob(const double val):val(val) {
}


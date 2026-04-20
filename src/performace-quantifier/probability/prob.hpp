#ifndef INC_2026_JAROS_PROB_HPP
#define INC_2026_JAROS_PROB_HPP


#include <stdexcept>
#include <iostream>


using val_t = double;

struct prob {
    prob operator+(const prob &other) const;

    prob &operator+=(const prob &other);

    prob &operator-=(prob other);

    prob &operator*=(prob other);

    prob &operator/=(prob other);

    prob operator-(const prob &other) const;

    prob operator*(const prob &other) const;

    prob operator/(const prob &other) const;

    bool operator==(const prob &other) const;

    bool operator!=(prob other) const;

    bool operator<(const prob &other) const;

    bool operator>(const prob &other) const;

    bool operator<=(const prob &other) const;

    bool operator>=(const prob &other) const;

    explicit prob(double val = 0.0);

    friend prob min(const prob &a, const prob &b);

    friend prob max(const prob &a, const prob &b);

    explicit operator double() const;

    friend std::ostream &operator<<(std::ostream &os, const prob &val);

private:
    val_t val;
};


#endif //INC_2026_JAROS_PROB_HPP

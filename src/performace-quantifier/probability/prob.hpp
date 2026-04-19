#ifndef INC_2026_JAROS_PROB_HPP
#define INC_2026_JAROS_PROB_HPP


using val_t = double;
struct prob {
    prob operator+(const prob& other) const;

    prob& operator+=(const prob& other);

    prob operator-(const prob& other) const;

    prob operator*(const prob& other) const;

    prob operator/(const prob& other) const;

    bool operator==(const prob &other) const;

    bool operator<(const prob& other) const;

    bool operator>(const prob& other) const;

    explicit prob(double val = 0.0);

private:
    val_t val;
};


#endif //INC_2026_JAROS_PROB_HPP
#ifndef INC_2026_JAROS_PERIODICPOLICY_HPP
#define INC_2026_JAROS_PERIODICPOLICY_HPP
#include <vector>


class periodicPolicy {
    std::vector<std::size_t> pol;
    std::size_t currIndex = 0;

public:
    using value_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    value_type operator*() const;

    periodicPolicy &operator++();

    void operator++(int);

    void reset();

    explicit periodicPolicy(std::vector<std::size_t> &&vec);

    explicit periodicPolicy(const std::vector<std::size_t> &vec);

    [[nodiscard]] std::size_t len() const;
};


#endif //INC_2026_JAROS_PERIODICPOLICY_HPP

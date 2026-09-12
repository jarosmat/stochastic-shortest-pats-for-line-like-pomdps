#ifndef INC_2026_JAROS_PERIODICPOLICY_HPP
#define INC_2026_JAROS_PERIODICPOLICY_HPP
#include <vector>


class periodic_policy {
    std::vector<std::size_t> pol_;
    std::size_t curr_index_ = 0;

public:
    using value_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    value_type operator*() const;

    periodic_policy &operator++();

    void operator++(int);

    void reset();

    explicit periodic_policy(std::vector<std::size_t> &&vec);

    explicit periodic_policy(const std::vector<std::size_t> &vec);

    [[nodiscard]] std::size_t len() const;
};


#endif //INC_2026_JAROS_PERIODICPOLICY_HPP

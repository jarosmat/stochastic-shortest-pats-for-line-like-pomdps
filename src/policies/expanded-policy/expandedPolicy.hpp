#ifndef INC_2026_JAROS_EXPANDED_POLICY_HPP
#define INC_2026_JAROS_EXPANDED_POLICY_HPP
#include <vector>


class expanded_policy {
    std::vector<std::size_t> pol_;
    std::size_t current_index_{0};
    std::size_t to_return_{0};
    bool is_first_{true};
    std::size_t max_val_;

    bool is_last_val() const;

public:
    using value_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    value_type operator*() const;

    expanded_policy &operator++();

    void operator++(int);

    void reset();

    explicit expanded_policy(std::vector<std::size_t> &&vec, std::size_t max_v);

    explicit expanded_policy(const std::vector<std::size_t> &vec, std::size_t max_v);
};


#endif //INC_2026_JAROS_EXPANDED_POLICY_HPP

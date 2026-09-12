#ifndef INC_2026_JAROS_PERMUTATIONS_WITH_REPETITION_HPP
#define INC_2026_JAROS_PERMUTATIONS_WITH_REPETITION_HPP

#include <vector>
#include <algorithm>
#include <ranges>
#include <utility>

template<typename T>
struct pair_first;

template<typename T1, typename T2>
struct pair_first<std::pair<T1, T2>> {
    using type = T1;
};

template<typename P>
using pair_first_t = pair_first<P>::type;

namespace detail {
    template<std::ranges::view V>
    class permutation_iterator {
        using val_t = pair_first_t<std::ranges::range_value_t<V>>;
        std::vector<val_t> current_perm_;
        bool is_at_end_ = false;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::vector<val_t>;
        using difference_type = std::ptrdiff_t;
        using reference = const value_type &;
        using pointer = void;


        permutation_iterator() = default;

        explicit permutation_iterator(V elements) {
            for (auto &&[item, count]: elements) {
                for (auto i{0uz}; i < static_cast<std::size_t>(count); ++i) {
                    current_perm_.push_back(item);
                }
            }

            std::ranges::sort(current_perm_);

            if (current_perm_.empty()) {
                is_at_end_ = true;
            }
        }

        const value_type &operator*() const {
            return current_perm_;
        }

        permutation_iterator &operator++() {
            if (!std::ranges::next_permutation(current_perm_).found) {
                is_at_end_ = true;
            }
            return *this;
        }

        permutation_iterator operator++(int) {
            permutation_iterator temp = *this;
            ++*this;
            return temp;
        }

        friend bool operator==(const permutation_iterator &it, const std::default_sentinel_t) {
            return it.is_at_end_;
        }
    };
}

template<std::ranges::range R>
class permutations_with_repetition_view
        : public std::ranges::view_interface<permutations_with_repetition_view<R> > {
    R base_;

public:
    using iterator = detail::permutation_iterator<R>;
    using sentinel = std::default_sentinel_t;

    permutations_with_repetition_view() = default;

    explicit permutations_with_repetition_view(R base)
        : base_(std::move(base)) {
    }

    iterator begin() {
        return iterator(base_);
    }

    iterator begin() const {
        return iterator(base_);
    }

    sentinel end() {
        return sentinel{};
    }

    [[nodiscard]] sentinel end() const {
        return sentinel{};
    }
};

template<class R>
permutations_with_repetition_view(R &&) -> permutations_with_repetition_view<std::ranges::views::all_t<R> >;

struct permutations_with_repetition : std::ranges::range_adaptor_closure<permutations_with_repetition> {
    template<std::ranges::viewable_range R>
    constexpr auto operator()(R &&rng) const {
        return permutations_with_repetition_view(std::views::all(std::forward<R>(rng)));
    }
};

#endif //INC_2026_JAROS_PERMUTATIONS_WITH_REPETITION_HPP

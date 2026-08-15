#ifndef INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#define INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#include <vector>
#include <ranges>


namespace detail {
    template<std::ranges::view V>
    class combination_iterator {
        using val_t = std::ranges::range_value_t<V>;
        V elements_;
        std::vector<std::size_t> indices_;
        std::size_t r_ = 0;
        std::vector<val_t> current_;
        bool done_ = true;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::vector<val_t>;
        using difference_type = std::ptrdiff_t;
        using reference = const value_type &;
        using pointer = void;

        combination_iterator() = default;

        combination_iterator(V elements, const std::size_t r)
            : elements_(std::move(elements)), indices_(r, 0), r_(r), done_(r == 0) {
            if (!done_ && r_ > 0) {
                auto first_it = std::ranges::begin(elements_);
                current_.resize(r_, *first_it);
            }
        }

        const value_type &operator*() const {
            return current_;
        }

        combination_iterator &operator++() {
            if (done_) {
                return *this;
            }

            const std::size_t elements_size = std::ranges::distance(elements_);
            std::size_t i{r_};
            while (i > 0 && ++indices_[i - 1] == elements_size) {
                i--;
            }

            if (i == 0) {
                done_ = true;
            } else {
                for (auto j{i}; j < r_; ++j) {
                    indices_[j] = indices_[i - 1];
                }
            }

            current_.clear();
            auto start_it = std::ranges::begin(elements_);
            for (auto l{0uz}; l < r_; ++l) {
                current_.push_back(*std::next(start_it, indices_[l]));
            }
            return *this;
        }

        void operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool operator==(const combination_iterator &it, const std::default_sentinel_t &) {
            return it.done_;
        }
    };
}

template<std::ranges::range R>
class combinations_with_repetition_view : public std::ranges::view_interface<combinations_with_repetition_view<R> > {
    R base_;
    std::size_t r_ = 0;

public:
    using iterator = detail::combination_iterator<R>;
    using sentinel = std::default_sentinel_t;

    combinations_with_repetition_view() = default;

    combinations_with_repetition_view(R base, const std::size_t r)
        : base_(std::move(base)), r_(r) {
    }

    iterator begin() {
        return iterator(base_, r_);
    }

    iterator begin() const {
        return iterator(base_, r_);
    }

    [[nodiscard]] sentinel end() {
        return sentinel{};
    }

    [[nodiscard]] sentinel end() const {
        return sentinel{};
    }
};


template<class R>
combinations_with_repetition_view(
    R &&, std::size_t) -> combinations_with_repetition_view<std::ranges::views::all_t<R> >;

struct combinations_with_repetition : std::ranges::range_adaptor_closure<combinations_with_repetition> {
    explicit combinations_with_repetition(const std::size_t r) : r_(r) {
    }

    template<std::ranges::viewable_range R>
    constexpr auto operator()(R &&rng) const {
        return combinations_with_repetition_view(std::views::all(std::forward<R>(rng)), r_);
    }

private:
    std::size_t r_;
};

#endif //INC_2026_JAROS_COMBINATIONS_WITH_REPETITION

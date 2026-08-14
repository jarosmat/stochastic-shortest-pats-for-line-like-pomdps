#ifndef INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#define INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#include <vector>
#include <ranges>


namespace detail {
    class empty_sentinel {
    };


    template<std::ranges::range R>
    class combination_iterator {
        using val_t = std::ranges::range_value_t<R>;
        const R &elements_;
        std::vector<std::size_t> indices_;
        std::size_t r_;
        bool done_;
        std::vector<val_t> current_;

    public:
        combination_iterator(const std::ranges::range auto &elements, const std::size_t r)
            : elements_(elements), indices_(r, 0), r_(r), done_(r == 0), current_(r, elements[0]) {
        }

        const std::vector<val_t> &operator*() const {
            return current_;
        }

        combination_iterator &operator++() {
            if (done_) {
                return *this;
            }

            std::size_t i{r_};
            while (i > 0 && ++indices_[i - 1] == elements_.size()) {
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
            for (auto l{0uz}; l < r_; ++l) {
                current_.push_back(std::ranges::begin(elements_)[indices_[l]]);
            }
            return *this;
        }

        bool operator==(const empty_sentinel &other) const {
            return done_;
        }
    };
}

template<std::ranges::view V>
class combinations_with_repetition_view
        : public std::ranges::view_interface<combinations_with_repetition_view<V> > {
    V base_;
    std::size_t r_;

public:
    combinations_with_repetition_view(V base, const std::size_t r)
        : base_(std::move(base)), r_(r) {
    }

    auto begin() const {
        return detail::combination_iterator<V>(
            base_, r_);
    }

    auto end() const {
        return detail::empty_sentinel{};
    }
};

template<std::ranges::range R>
auto combinations_with_repetition(R &&rng, std::size_t r) {
    return combinations_with_repetition_view<
        std::ranges::views::all_t<R> >(
        std::forward<R>(rng), r);
}


#endif //INC_2026_JAROS_COMBINATIONS_WITH_REPETITION

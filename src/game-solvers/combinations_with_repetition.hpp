#ifndef INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#define INC_2026_JAROS_COMBINATIONS_WITH_REPETITION
#include <vector>

template<typename T>
class combination_iterator {
    const std::vector<T> &elements_;
    std::vector<size_t> indices_;
    std::size_t r_ = 0;
    bool done_ = true;
    mutable std::vector<T> current_;

public:
    combination_iterator(const std::vector<T> &elements, std::size_t r)
        : elements_(&elements), indices_(r, 0), r_(r), done_(r == 0) {
    }

    const std::vector<T> &operator*() const {
        current_.clear();
        for (auto i{0uz}; i < r_; ++i) {
            current_.push_back((*elements_)[indices_[i]]);
        }

        return current_;
    }

    combination_iterator &operator++() {
        if (done_) {
            return *this;
        };

        std::size_t i{r_};
        while (i > 0 && ++indices_[i - 1] == elements_.size()) {
            i--;
        }

        if (i == 0) {
            done_ = true;
        } else {
            for (auto j{i}; j < r_; ++j) {
                indices_[j] = indices_[i];
            }
        }

        return *this;
    }

    bool operator!=(const combination_iterator &other) const {
        return done_ != other.done_;
    }
};

#endif //INC_2026_JAROS_COMBINATIONS_WITH_REPETITION

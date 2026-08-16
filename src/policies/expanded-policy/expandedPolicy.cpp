#include "expandedPolicy.hpp"
#include <algorithm>


bool expanded_policy::is_last_val() const {
    if (pol_.empty()) {
        return to_return_ == max_val_;
    }
    if (max_val_ <= pol_.back()) {
        return current_index_ == pol_.size() - 1 && !is_first_;
    }

    return to_return_ == max_val_;
}

expanded_policy::value_type expanded_policy::operator*() const {
    return to_return_;
}

expanded_policy &expanded_policy::operator++() {
    if (is_last_val()) {
        reset();
        return *this;
    }
    if (pol_.empty()) {
        ++to_return_;
        return *this;
    }

    if (current_index_ == pol_.size()) {
        ++to_return_;
        return *this;
    }
    if (is_first_ && to_return_ == pol_[current_index_]) {
        is_first_ = false;
        return *this;
    }
    if (is_first_) {
        ++to_return_;
        return *this;
    }

    ++current_index_;
    if (current_index_ == pol_.size() || to_return_ != pol_[current_index_]) {
        ++to_return_;
        is_first_ = true;
    }

    return *this;
}

void expanded_policy::operator++(int) {
    ++*this;
}

void expanded_policy::reset() {
    current_index_ = 0;
    to_return_ = 0;
    is_first_ = true;
}

expanded_policy::expanded_policy(std::vector<std::size_t> &&vec, const std::size_t max_v) : pol_(std::move(vec)),
    max_val_(max_v) {
}

expanded_policy::expanded_policy(const std::vector<std::size_t> &vec,
                               const std::size_t max_v) : pol_(vec), max_val_(max_v) {
}

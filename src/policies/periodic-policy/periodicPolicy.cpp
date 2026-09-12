#include "periodicPolicy.hpp"

periodic_policy::value_type periodic_policy::operator*() const {
    return pol_[curr_index_];
}

periodic_policy &periodic_policy::operator++() {
    if (++curr_index_ >= pol_.size()) {
        curr_index_ = 0;
    }

    return *this;
}

void periodic_policy::operator++(int) {
    ++*this;
}

void periodic_policy::reset() {
    curr_index_ = 0;
}

periodic_policy::periodic_policy(std::vector<std::size_t> &&vec): pol_(std::move(vec)) {
}

periodic_policy::periodic_policy(const std::vector<std::size_t> &vec): pol_(vec) {
}

std::size_t periodic_policy::len() const {
    return pol_.size();
}

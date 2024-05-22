#pragma once

#include <algorithm>
#include <memory>
#include <vector>

template <typename T>
int findIndexOfWeakPtr(std::vector<std::weak_ptr<T>> const& vec, std::weak_ptr<T> const& target) {
    auto targetShared = target.lock();
    if (!targetShared) {
        // If the target weak_ptr is expired, it can't be found in the vector
        return -1;
    }

    auto it = std::find_if(vec.begin(), vec.end(), [&targetShared](std::weak_ptr<T> const& wptr) {
        auto shared = wptr.lock();
        return shared && shared == targetShared;
    });

    if (it != vec.end()) {
        return static_cast<int>(std::distance(vec.begin(), it));
    } else {
        return -1;    // Not found
    }
}

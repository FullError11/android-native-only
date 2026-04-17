#pragma once

#include <optional>
#include <initializer_list>
#include <map>
#include <functional>
#include <stdexcept>
#include <tuple>

namespace ano::detail {

template<typename KeyT, typename ValueT>
class RangeMap {
    std::map<KeyT, std::pair<KeyT, ValueT>> intervals;

public:
    // { {start, end}, value }
    RangeMap(std::initializer_list<std::pair<std::pair<KeyT, KeyT>, ValueT>> init) {
        for (const auto& [range, val] : init) {
            const auto& [start, end] = range;
            addInterval(start, end, val);
        }
        validateNonEmpty();
    }

    // { start, end, value }
    RangeMap(std::initializer_list<std::tuple<KeyT, KeyT, ValueT>> init) {
        for (const auto& [start, end, val] : init) {
            addInterval(start, end, val);
        }
        validateNonEmpty();
    }

    std::optional<std::reference_wrapper<ValueT>> get(const KeyT& k) {
        auto it = intervals.upper_bound(k);
        if (it == intervals.begin()) {
            return std::nullopt;
        }
        --it;
        
        if (k <= it->second.first) {
            return std::ref(it->second.second);
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const ValueT>> get(const KeyT& k) const {
        auto it = intervals.upper_bound(k);
        if (it == intervals.begin()) {
            return std::nullopt;
        }
        --it;
        
        if (k <= it->second.first) {
            return std::cref(it->second.second);
        }
        return std::nullopt;
    }

private:
    void addInterval(const KeyT& start, const KeyT& end, const ValueT& val) {
        if (end < start) {
            throw std::invalid_argument("Invalid range: end must not be less than start");
        }
        intervals.emplace(start, std::make_pair(end, val));
    }

    void validateNonEmpty() {
        if (intervals.empty()) {
            throw std::invalid_argument("RangeMap must be non-empty");
        }
    }
};

}

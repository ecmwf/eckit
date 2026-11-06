// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Policy.h"

#include <tuple>
#include <utility>

#include "eckit/cache/Exceptions.h"


namespace eckit::cache {


namespace {


constexpr bool least_recently_used(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return a.last_use < b.last_use;
}


constexpr bool least_frequently_used(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return std::tie(a.uses, a.last_use) < std::tie(b.uses, b.last_use);
}


constexpr bool first_in_first_out(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return a.inserted < b.inserted;
}


constexpr bool largest(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    // Largest first (then least recently used)
    const auto x = a.footprint.total();
    const auto y = b.footprint.total();
    return x > y || (x == y && a.last_use < b.last_use);
}


// Constant, as a MemoryCache can be constructed during static initialisation of another translation unit
constexpr std::pair<std::string_view, Policy::EvictsBefore> POLICIES[]{
    {"fifo", &first_in_first_out},
    {"largest", &largest},
    {"lfu", &least_frequently_used},
    {"lru", &least_recently_used},
};


}  // namespace


Policy Policy::build(std::string_view name) {
    for (const auto& [key, evicts_before] : POLICIES) {
        if (key == name) {
            return {key, evicts_before};
        }
    }

    std::string choices;
    for (const auto& [key, evicts_before] : POLICIES) {
        choices += (choices.empty() ? "" : ", ") + std::string{key};
    }

    throw CacheError("Policy: unknown '" + std::string{name} + "', choices are: " + choices, Here());
}


std::vector<std::string> Policy::names() {
    std::vector<std::string> names;
    for (const auto& [key, evicts_before] : POLICIES) {
        names.emplace_back(key);
    }
    return names;
}


}  // namespace eckit::cache

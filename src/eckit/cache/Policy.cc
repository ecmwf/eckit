// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Policy.h"

#include <utility>

#include "eckit/cache/Exceptions.h"


namespace eckit::cache {


namespace {


// Least recently used first, as values used recently are likely used again (the default)
constexpr bool least_recently_used(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return a.last_use < b.last_use;
}


// Most recently used first, for cycles over more values than fit, where the least recently used is needed next
constexpr bool most_recently_used(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return least_recently_used(b, a);
}


// Used once first, then least recently used, so one pass over many values doesn't evict those used again
constexpr bool used_once(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    const bool a1 = a.uses == 1;
    const bool b1 = b.uses == 1;
    return a1 != b1 ? a1 : least_recently_used(a, b);
}


// Least frequently used first, then least recently used, keeping values used often even if not recently
constexpr bool least_frequently_used(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return a.uses < b.uses || (a.uses == b.uses && least_recently_used(a, b));
}


// First inserted first, regardless of use, so the order of eviction is predictable
constexpr bool first_in_first_out(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    return a.inserted < b.inserted;
}


// Largest first, then least recently used, making room with the fewest evictions
constexpr bool largest(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    const auto af = a.footprint.total();
    const auto bf = b.footprint.total();
    return af > bf || (af == bf && least_recently_used(a, b));
}


// Largest per use first, then least recently used, as largest but keeping large values used often
constexpr bool largest_per_use(const Policy::Entry& a, const Policy::Entry& b) noexcept {
    const auto afu = a.footprint.total() * b.uses;
    const auto bfu = b.footprint.total() * a.uses;
    return afu > bfu || (afu == bfu && least_recently_used(a, b));
}


constexpr std::pair<std::string_view, Policy::EvictsBefore> POLICIES[]{
    {"fifo", &first_in_first_out},          //
    {"largest", &largest},                  //
    {"largest-per-use", &largest_per_use},  //
    {"lfu", &least_frequently_used},        //
    {"lru", &least_recently_used},          //
    {"mru", &most_recently_used},           //
    {"used-once", &used_once},              //
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

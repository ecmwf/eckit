// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Policy.h"

#include <map>

#include "eckit/cache/Exceptions.h"


namespace eckit::cache {


const std::string Policy::DEFAULT = "lru";


namespace {


/// Least recently used first (the default)
struct LeastRecentlyUsed final : Policy {
    bool before(const Entry& a, const Entry& b) const override { return a.last_use < b.last_use; }
    const std::string& name() const override {
        static const std::string NAME{"lru"};
        return NAME;
    }
};


/// Least used first (then least recently used)
struct LeastFrequentlyUsed final : Policy {
    bool before(const Entry& a, const Entry& b) const override {
        return a.uses < b.uses || (a.uses == b.uses && a.last_use < b.last_use);
    }
    const std::string& name() const override {
        static const std::string NAME{"lfu"};
        return NAME;
    }
};


/// Oldest first (by insertion)
struct FirstInFirstOut final : Policy {
    bool before(const Entry& a, const Entry& b) const override { return a.inserted < b.inserted; }
    const std::string& name() const override {
        static const std::string NAME{"fifo"};
        return NAME;
    }
};


/// Largest first (then least recently used)
struct Largest final : Policy {
    bool before(const Entry& a, const Entry& b) const override {
        const auto x = a.footprint.total();
        const auto y = b.footprint.total();
        return x > y || (x == y && a.last_use < b.last_use);
    }
    const std::string& name() const override {
        static const std::string NAME{"largest"};
        return NAME;
    }
};


template <typename P>
Policy* make() {
    return new P;
}


const std::map<std::string, Policy* (*)()> POLICIES{
    {"lru", &make<LeastRecentlyUsed>},
    {"lfu", &make<LeastFrequentlyUsed>},
    {"fifo", &make<FirstInFirstOut>},
    {"largest", &make<Largest>},
};


}  // namespace


Policy* Policy::build(const std::string& name) {
    if (auto it = POLICIES.find(name); it != POLICIES.end()) {
        return it->second();
    }

    std::string choices;
    for (const auto& n : names()) {
        choices += (choices.empty() ? "" : ", ") + n;
    }

    throw CacheError("Policy: unknown '" + name + "', choices are: " + choices, Here());
}


std::vector<std::string> Policy::names() {
    std::vector<std::string> names;
    for (const auto& [name, make] : POLICIES) {
        names.push_back(name);
    }
    return names;
}


}  // namespace eckit::cache

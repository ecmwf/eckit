// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <algorithm>
#include <iomanip>
#include <ios>
#include <map>
#include <memory>
#include <ostream>
#include <regex>
#include <string>
#include <utility>

#include "eckit/spec/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::spec {
class Custom;
class Spec;
}  // namespace eckit::spec


namespace eckit::spec {

//------------------------------------------------------------------------------------------------------

struct lock_type {
    explicit lock_type(Mutex& mutex) : lock_guard_{mutex} {}
    AutoLock<Mutex> lock_guard_;
};

//------------------------------------------------------------------------------------------------------

template <class C>
class GeneratorT {
public:

    // -- Types

    using generator_t = C;
    using key_t       = std::string;
    using storage_t   = std::map<key_t, std::shared_ptr<generator_t>>;  // shared, so in use while unregistered

    // -- Constructors

    GeneratorT(const GeneratorT&) = delete;
    GeneratorT(GeneratorT&&)      = delete;

    // -- Operators

    void operator=(const GeneratorT&) = delete;
    void operator=(GeneratorT&&)      = delete;

    // -- Methods

    static GeneratorT<C>& instance();

    bool exists(const key_t&) const;
    bool matches(const std::string&) const;

    void regist(const key_t&, generator_t*);
    void unregist(const key_t&);

    void replace(const key_t&, generator_t*);

    const generator_t& get(const key_t& k) const { return *get_shared(k); }
    const generator_t& match(const std::string& k) const { return *match_shared(k); }

    template <typename... Args>
    [[nodiscard]] auto get_spec(const key_t& k, Args&&... args) const {
        return get_shared(k)->spec(std::forward<Args>(args)...);
    }

    template <typename... Args>
    [[nodiscard]] auto match_spec(const std::string& k, Args&&... args) const {
        return match_shared(k)->spec(std::forward<Args>(args)...);
    }

    const storage_t& store() const { return store_; }

    bool match(const Custom& spec, std::string& name) const {
        lock_type lock(mutex_);

        auto end = store_.cend();
        auto i   = end;
        for (auto j = store_.cbegin(); j != end; ++j) {
            if (!(j->first.empty()) && j->second->match(spec)) {
                if (i != end) {
                    throw SeriousBug("Generator matches names '" + i->first + "' and '" + j->first + "'", Here());
                }
                i = j;
            }
        }

        if (i != end) {
            name = i->first;
            ASSERT(!name.empty());
            return true;
        }

        return false;
    }

private:

    // -- Constructors

    GeneratorT() = default;

    // -- Members

    mutable Mutex mutex_;
    storage_t store_;

    // -- Methods

    std::shared_ptr<const generator_t> get_shared(const key_t&) const;
    std::shared_ptr<const generator_t> match_shared(const std::string&) const;

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const GeneratorT<C>& o) {
        o.print(os);
        return os;
    }
};

//------------------------------------------------------------------------------------------------------

template <class C>
GeneratorT<C>& GeneratorT<C>::instance() {
    static GeneratorT<C> obj;
    return obj;
}

template <class C>
bool GeneratorT<C>::exists(const key_t& k) const {
    lock_type lock(mutex_);

    return store_.find(k) != store_.end();
}

template <class C>
bool GeneratorT<C>::matches(const std::string& k) const {
    lock_type lock(mutex_);

    return std::any_of(store_.begin(), store_.end(), [&](const auto& p) -> bool {
        const std::regex rex(p.first, std::regex_constants::icase);
        return std::regex_match(k, rex);
    });
}

template <class C>
void GeneratorT<C>::regist(const key_t& k, generator_t* c) {
    lock_type lock(mutex_);

    if (exists(k)) {
        throw BadParameter("Generator has already a builder for " + k, Here());
    }

    ASSERT(c != nullptr);
    store_[k].reset(c);
}

template <class C>
void GeneratorT<C>::replace(const key_t& k, generator_t* c) {
    lock_type lock(mutex_);

    ASSERT(c != nullptr);
    store_[k].reset(c);
}

template <class C>
void GeneratorT<C>::unregist(const key_t& k) {
    lock_type lock(mutex_);

    if (auto it = store_.find(k); it != store_.end()) {
        store_.erase(it);
        return;
    }

    throw BadParameter("Generator unknown: '" + k + "'", Here());
}

template <class C>
std::shared_ptr<const typename GeneratorT<C>::generator_t> GeneratorT<C>::get_shared(const key_t& k) const {
    lock_type lock(mutex_);

    if (auto it = store_.find(k); it != store_.end()) {
        return it->second;
    }

    throw BadParameter("Generator unknown: '" + k + "'", Here());
}

template <class C>
std::shared_ptr<const typename GeneratorT<C>::generator_t> GeneratorT<C>::match_shared(const std::string& k) const {
    lock_type lock(mutex_);

    auto end = store_.cend();
    auto i   = end;
    for (auto j = store_.cbegin(); j != end; ++j) {
        if (const std::regex rex(j->first, std::regex_constants::icase); std::regex_match(k, rex)) {
            if (i != end) {
                throw SeriousBug("Generator name '" + k + "' matches '" + i->first + "' and '" + j->first + "'",
                                 Here());
            }
            i = j;
        }
    }

    if (i != end) {
        return i->second;
    }

    throw BadParameter("Generator unknown: '" + k + "'", Here());
}

template <class C>
void GeneratorT<C>::print(std::ostream& os) const {
    lock_type lock(mutex_);

    os << "Generator" << std::endl;

    int key_width = 0;
    for (const auto& i : store_) {
        key_width = std::max(static_cast<int>(i.first.size()), key_width);
    }

    for (const auto& i : store_) {
        os << "    " << std::setw(key_width) << std::left << i.first << "  --  " << i.second.get() << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------

class SpecGenerator {
public:

    // -- Types

    using key_t = std::string;

    static constexpr const char* uid_pattern = "[0-9a-fA-F]{32}";

    // -- Constructors

    SpecGenerator()                     = default;
    SpecGenerator(const SpecGenerator&) = delete;
    SpecGenerator(SpecGenerator&&)      = delete;

    // -- Destructor

    virtual ~SpecGenerator() = default;

    // -- Operators

    void operator=(const SpecGenerator&) = delete;
    void operator=(SpecGenerator&&)      = delete;

    // -- Methods

    virtual bool match(const spec::Custom&) const { return false; }
};

//------------------------------------------------------------------------------------------------------

struct SpecGeneratorT0 : public SpecGenerator {
    [[nodiscard]] virtual Spec* spec() const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1>
struct SpecGeneratorT1 : public SpecGenerator {
    using arg1_t = ARG1;

    [[nodiscard]] virtual Spec* spec(arg1_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1, typename ARG2>
struct SpecGeneratorT2 : public SpecGenerator {
    using arg1_t = ARG1;
    using arg2_t = ARG2;

    [[nodiscard]] virtual Spec* spec(arg1_t, arg2_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <class T>
struct ConcreteSpecGeneratorT0 final : public SpecGeneratorT0 {
    explicit ConcreteSpecGeneratorT0(const typename SpecGeneratorT0::key_t& key) {
        GeneratorT<SpecGeneratorT0>::instance().regist(key, this);
    }

    [[nodiscard]] Spec* spec() const override { return T::spec(); }
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1>
struct ConcreteSpecGeneratorT1 final : public SpecGeneratorT1<ARG1> {
    explicit ConcreteSpecGeneratorT1(const typename SpecGeneratorT1<ARG1>::key_t& key) {
        GeneratorT<SpecGeneratorT1<ARG1>>::instance().regist(key, this);
    }

    [[nodiscard]] Spec* spec(typename SpecGeneratorT1<ARG1>::arg1_t p1) const override { return T::spec(p1); }
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1, typename ARG2>
struct ConcreteSpecGeneratorT2 final : public SpecGeneratorT2<ARG1, ARG2> {
    explicit ConcreteSpecGeneratorT2(const typename SpecGeneratorT2<ARG1, ARG2>::key_t& key) {
        GeneratorT<SpecGeneratorT2<ARG1, ARG2>>::instance().regist(key, this);
    }

    [[nodiscard]] Spec* spec(typename SpecGeneratorT1<ARG1>::arg1_t p1,
                             typename SpecGeneratorT1<ARG1>::arg2_t p2) const override {
        return T::spec(p1, p2);
    }
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::spec

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo {
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo::spec {

//------------------------------------------------------------------------------------------------------

template <class C>
class GeneratorT {
public:

    // -- Types

    using generator_t = C;
    using key_t       = std::string;
    using storage_t   = std::map<key_t, generator_t*>;

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

    const generator_t& get(const key_t&) const;
    const generator_t& match(const std::string&) const;

    bool match(const Custom& spec, std::string& name) const {
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

    // -- Destructor

    ~GeneratorT() = default;

    // -- Members

    mutable Mutex mutex_;
    storage_t store_;

    // -- Methods

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const GeneratorT<C>& o) {
        o.print(os);
        return os;
    }
};

//------------------------------------------------------------------------------------------------------

struct lock_type {
    explicit lock_type(util::recursive_mutex& mutex) : lock_guard_{mutex} {}
    util::lock_guard<util::recursive_mutex> lock_guard_;
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
    store_[k] = c;
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
const typename GeneratorT<C>::generator_t& GeneratorT<C>::get(const key_t& k) const {
    lock_type lock(mutex_);

    if (auto it = store_.find(k); it != store_.end()) {
        return *(it->second);
    }
    throw BadParameter("Generator unknown: '" + k + "'", Here());
}

template <class C>
const typename GeneratorT<C>::generator_t& GeneratorT<C>::match(const std::string& k) const {
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
        return *(i->second);
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
        os << "    " << std::setw(key_width) << std::left << i.first << "  --  " << i.second << std::endl;
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

class SpecGeneratorT0 : public SpecGenerator {
public:

    // -- Methods

    [[nodiscard]] virtual Spec* spec() const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1>
class SpecGeneratorT1 : public SpecGenerator {
public:

    // -- Types

    using arg1_t = ARG1;

    // -- Methods

    [[nodiscard]] virtual Spec* spec(arg1_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1, typename ARG2>
class SpecGeneratorT2 : public SpecGenerator {
public:

    // -- Types

    using arg1_t = ARG1;
    using arg2_t = ARG2;

    // -- Methods

    [[nodiscard]] virtual Spec* spec(arg1_t, arg2_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <class T>
class ConcreteSpecGeneratorT0 final : public SpecGeneratorT0 {
public:

    // -- Constructors

    explicit ConcreteSpecGeneratorT0(const SpecGeneratorT0::key_t& k) : key_(k) {
        GeneratorT<SpecGeneratorT0>::instance().regist(key_, this);
    }

    ConcreteSpecGeneratorT0(const ConcreteSpecGeneratorT0&) = delete;
    ConcreteSpecGeneratorT0(ConcreteSpecGeneratorT0&&)      = delete;

    // -- Destructor

    ~ConcreteSpecGeneratorT0() override { GeneratorT<SpecGeneratorT0>::instance().unregist(key_); }

    // -- Operators

    void operator=(const ConcreteSpecGeneratorT0&) = delete;
    void operator=(ConcreteSpecGeneratorT0&&)      = delete;

    // -- Overridden methods

    [[nodiscard]] Spec* spec() const override { return T::spec(); }

private:

    // -- Members

    SpecGeneratorT0::key_t key_;
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1>
class ConcreteSpecGeneratorT1 final : public SpecGeneratorT1<ARG1> {
public:

    // -- Constructors

    explicit ConcreteSpecGeneratorT1(const typename SpecGeneratorT1<ARG1>::key_t& k) : key_(k) {
        GeneratorT<SpecGeneratorT1<ARG1>>::instance().regist(key_, this);
    }

    ConcreteSpecGeneratorT1(const ConcreteSpecGeneratorT1&) = delete;
    ConcreteSpecGeneratorT1(ConcreteSpecGeneratorT1&&)      = delete;

    // -- Destructor

    ~ConcreteSpecGeneratorT1() override { GeneratorT<SpecGeneratorT1<ARG1>>::instance().unregist(key_); }

    // -- Operators

    void operator=(const ConcreteSpecGeneratorT1&) = delete;
    void operator=(ConcreteSpecGeneratorT1&&)      = delete;

    // -- Overridden methods

    [[nodiscard]] Spec* spec(typename SpecGeneratorT1<ARG1>::arg1_t p1) const override { return T::spec(p1); }

private:

    // -- Members

    typename SpecGeneratorT1<ARG1>::key_t key_;
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1, typename ARG2>
class ConcreteSpecGeneratorT2 final : public SpecGeneratorT2<ARG1, ARG2> {
public:

    // -- Constructors

    explicit ConcreteSpecGeneratorT2(const typename SpecGeneratorT2<ARG1, ARG2>::key_t& k) : key_(k) {
        GeneratorT<SpecGeneratorT2<ARG1, ARG2>>::instance().regist(key_, this);
    }

    ConcreteSpecGeneratorT2(const ConcreteSpecGeneratorT2&) = delete;
    ConcreteSpecGeneratorT2(ConcreteSpecGeneratorT2&&)      = delete;

    // -- Destructor

    ~ConcreteSpecGeneratorT2() override { GeneratorT<SpecGeneratorT2<ARG1, ARG2>>::instance().unregist(key_); }

    // -- Operators

    void operator=(const ConcreteSpecGeneratorT2&) = delete;
    void operator=(ConcreteSpecGeneratorT2&&)      = delete;

    // -- Overridden methods

    [[nodiscard]] Spec* spec(typename SpecGeneratorT1<ARG1>::arg1_t p1,
                             typename SpecGeneratorT1<ARG1>::arg2_t p2) const override {
        return T::spec(p1, p2);
    }

private:

    // -- Members

    typename SpecGeneratorT2<ARG1, ARG2>::key_t key_;
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo::spec

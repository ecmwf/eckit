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

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Regex.h"


namespace eckit {
class Configuration;
}


namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

template <class C>
class Configurator {
public:
    // -- Types

    using configurator_t = C;
    using key_t          = std::string;
    using storage_t      = std::map<key_t, configurator_t*>;

    // -- Constructors

    Configurator(const Configurator&) = delete;
    Configurator(Configurator&&)      = delete;

    // -- Operators

    void operator=(const Configurator&) = delete;
    void operator=(Configurator&&)      = delete;

    // -- Methods

    static Configurator<C>& instance();

    bool exists(const key_t&) const;
    void regist(const key_t&, configurator_t*);
    void unregist(const key_t&);

    const configurator_t& get(const key_t&) const;
    const configurator_t& match(const std::string&) const;

private:
    // -- Constructors

    Configurator() = default;

    // -- Destructor

    ~Configurator() = default;

    // -- Members

    mutable Mutex mutex_;
    storage_t store_;

    // -- Methods

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Configurator<C>& o) {
        o.print(os);
        return os;
    }
};

//------------------------------------------------------------------------------------------------------

template <class C>
Configurator<C>& Configurator<C>::instance() {
    static Configurator<C> obj;
    return obj;
}

template <class C>
bool Configurator<C>::exists(const key_t& k) const {
    AutoLock<Mutex> lock(mutex_);
    return (store_.find(k) != store_.end());
}

template <class C>
void Configurator<C>::regist(const key_t& k, configurator_t* c) {
    AutoLock<Mutex> lock(mutex_);
    if (exists(k)) {
        throw BadParameter("Configurator has already a builder for " + k, Here());
    }
    ASSERT(c != nullptr);
    store_[k] = c;
}

template <class C>
void Configurator<C>::unregist(const key_t& k) {
    AutoLock<Mutex> lock(mutex_);
    if (!exists(k)) {
        throw BadParameter("Configurator unknown: '" + k + "'", Here());
    }
    store_.erase(k);
}

template <class C>
const typename Configurator<C>::configurator_t& Configurator<C>::get(const key_t& k) const {
    AutoLock<Mutex> lock(mutex_);
    if (!exists(k)) {
        throw BadParameter("Configurator unknown: '" + k + "'", Here());
    }
    return *(store_.find(k)->second);
}

template <class C>
const typename Configurator<C>::configurator_t& Configurator<C>::match(const std::string& k) const {
    AutoLock<Mutex> lock(mutex_);

    auto end = store_.cend();
    auto i   = end;
    for (auto j = store_.cbegin(); j != end; ++j) {
        if (Regex(j->first).match(k)) {
            if (i != end) {
                throw SeriousBug("Configurator name '" + k + "' matches '" + i->first + "' and '" + j->first + "'");
            }
            i = j;
        }
    }

    if (i != end) {
        return *(i->second);
    }

    throw BadParameter("Configurator unknown: '" + k + "'", Here());
}

template <class C>
void Configurator<C>::print(std::ostream& os) const {
    AutoLock<Mutex> lock(mutex_);
    os << "Configurator" << std::endl;

    int key_width = 0;
    for (const auto& i : store_) {
        key_width = std::max(static_cast<int>(i.first.size()), key_width);
    }

    for (const auto& i : store_) {
        os << "    " << std::setw(key_width) << std::left << i.first << "  --  " << i.second << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------

class ConfigurationGenerator {
public:
    // -- Types

    using key_t = std::string;

    // -- Constructors

    ConfigurationGenerator()                              = default;
    ConfigurationGenerator(const ConfigurationGenerator&) = delete;
    ConfigurationGenerator(ConfigurationGenerator&&)      = delete;

    // -- Destructor

    virtual ~ConfigurationGenerator() = default;

    // -- Operators

    void operator=(const ConfigurationGenerator&) = delete;
    void operator=(ConfigurationGenerator&&)      = delete;
};

//------------------------------------------------------------------------------------------------------

class ConfigurationGeneratorT0 : public ConfigurationGenerator {
public:
    // -- Methods

    virtual Configuration* config() const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1>
class ConfigurationGeneratorT1 : public ConfigurationGenerator {
public:
    // -- Types

    using arg1_t = ARG1;

    // -- Methods

    virtual Configuration* config(arg1_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <typename ARG1, typename ARG2>
class ConfigurationGeneratorT2 : public ConfigurationGenerator {
public:
    // -- Types

    using arg1_t = ARG1;
    using arg2_t = ARG2;

    // -- Methods

    virtual Configuration* config(arg1_t, arg2_t) const = 0;
};

//------------------------------------------------------------------------------------------------------

template <class T>
class ConcreteConfigurationGeneratorT0 final : public ConfigurationGeneratorT0 {
public:
    // -- Constructors

    explicit ConcreteConfigurationGeneratorT0(const ConfigurationGeneratorT0::key_t& k) :
        key_(k) {
        Configurator<ConfigurationGeneratorT0>::instance().regist(key_, this);
    }

    ConcreteConfigurationGeneratorT0(const ConcreteConfigurationGeneratorT0&) = delete;
    ConcreteConfigurationGeneratorT0(ConcreteConfigurationGeneratorT0&&)      = delete;

    // -- Destructor

    ~ConcreteConfigurationGeneratorT0() override {
        Configurator<ConfigurationGeneratorT0>::instance().unregist(key_);
    }

    // -- Operators

    void operator=(const ConcreteConfigurationGeneratorT0&) = delete;
    void operator=(ConcreteConfigurationGeneratorT0&&)      = delete;

    // -- Overridden methods

    Configuration* config() const override { return T::config(); }

private:
    // -- Members

    ConfigurationGeneratorT0::key_t key_;
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1>
class ConcreteConfigurationGeneratorT1 final : public ConfigurationGeneratorT1<ARG1> {
public:
    // -- Constructors

    explicit ConcreteConfigurationGeneratorT1(const typename ConfigurationGeneratorT1<ARG1>::key_t& k) :
        key_(k) {
        Configurator<ConfigurationGeneratorT1<ARG1>>::instance().regist(key_, this);
    }

    ConcreteConfigurationGeneratorT1(const ConcreteConfigurationGeneratorT1&) = delete;
    ConcreteConfigurationGeneratorT1(ConcreteConfigurationGeneratorT1&&)      = delete;

    // -- Destructor

    ~ConcreteConfigurationGeneratorT1() override {
        Configurator<ConfigurationGeneratorT1<ARG1>>::instance().unregist(key_);
    }

    // -- Operators

    void operator=(const ConcreteConfigurationGeneratorT1&) = delete;
    void operator=(ConcreteConfigurationGeneratorT1&&)      = delete;

    // -- Overridden methods

    Configuration* config(typename ConfigurationGeneratorT1<ARG1>::arg1_t p1) const override { return T::config(p1); }

private:
    // -- Members

    typename ConfigurationGeneratorT1<ARG1>::key_t key_;
};

//------------------------------------------------------------------------------------------------------

template <class T, typename ARG1, typename ARG2>
class ConcreteConfigurationGeneratorT2 final : public ConfigurationGeneratorT2<ARG1, ARG2> {
public:
    // -- Constructors

    explicit ConcreteConfigurationGeneratorT2(const typename ConfigurationGeneratorT2<ARG1, ARG2>::key_t& k) :
        key_(k) {
        Configurator<ConfigurationGeneratorT2<ARG1, ARG2>>::instance().regist(key_, this);
    }

    ConcreteConfigurationGeneratorT2(const ConcreteConfigurationGeneratorT2&) = delete;
    ConcreteConfigurationGeneratorT2(ConcreteConfigurationGeneratorT2&&)      = delete;

    // -- Destructor

    ~ConcreteConfigurationGeneratorT2() override {
        Configurator<ConfigurationGeneratorT2<ARG1, ARG2>>::instance().unregist(key_);
    }

    // -- Operators

    void operator=(const ConcreteConfigurationGeneratorT2&) = delete;
    void operator=(ConcreteConfigurationGeneratorT2&&)      = delete;

    // -- Overridden methods

    Configuration* config(typename ConfigurationGeneratorT1<ARG1>::arg1_t p1, typename ConfigurationGeneratorT1<ARG1>::arg2_t p2) const override { return T::config(p1, p2); }

private:
    // -- Members

    typename ConfigurationGeneratorT2<ARG1, ARG2>::key_t key_;
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

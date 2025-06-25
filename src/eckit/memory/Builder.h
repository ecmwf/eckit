/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Builder.h
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Jul 2014


#pragma once

#include "eckit/eckit_config.h"
#include "eckit/memory/Factory.h"

#if eckit_HAVE_ECKIT_MEMORY_FACTORY_BUILDERS_DEBUG
#include "eckit/exception/Exceptions.h"
#define DEBUG_BUILDER(x) std::cerr << " DEBUG (" << x << ") " << Here() << std::endl;
#else
#define DEBUG_BUILDER(x)
#endif


namespace eckit {

//------------------------------------------------------------------------------------------------------

class Builder {
public:

    // -- Types

    using key_t = std::string;

    // -- Constructors

    Builder()               = default;
    Builder(const Builder&) = delete;
    Builder(Builder&&)      = delete;

    // -- Destructor

    virtual ~Builder() = default;

    // -- Operators

    void operator=(const Builder&) = delete;
    void operator=(Builder&&)      = delete;

    // -- Methods

    virtual key_t name() const       = 0;
    virtual key_t build_type() const = 0;

private:

    // -- Methods

    virtual void print(std::ostream& os) const { os << "Builder(" << build_type() << "):" << name(); }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Builder& o) {
        o.print(os);
        return os;
    }
};

//------------------------------------------------------------------------------------------------------

template <class Base>
class BuilderT0 : public Builder {
public:

    // -- Types

    using product_t = Base;

    // -- Methods

    virtual product_t* create() const = 0;

    // -- Overridden methods

    typename Builder::key_t build_type() const override { return Base::className(); }
};

//------------------------------------------------------------------------------------------------------

template <class Base>
class BuilderT1 : public Builder {
public:

    // -- Types

    using product_t = Base;

    using ARG1 = typename product_t::ARG1;

    // -- Methods

    virtual product_t* create(ARG1) const = 0;

    // -- Overridden methods

    typename Builder::key_t build_type() const override { return Base::className(); }
};

//------------------------------------------------------------------------------------------------------

template <class Base>
class BuilderT2 : public Builder {
public:

    // -- Types

    using product_t = Base;

    using ARG1 = typename product_t::ARG1;
    using ARG2 = typename product_t::ARG2;

    // -- Methods

    virtual product_t* create(ARG1, ARG2) const = 0;

    // -- Overridden methods

    typename Builder::key_t build_type() const override { return Base::className(); }
};


//------------------------------------------------------------------------------------------------------

template <class Base, class T>
class ConcreteBuilderT0 final : public BuilderT0<Base> {
public:

    // -- Types

    using base_t = BuilderT0<Base>;

    // -- Constructors

    ConcreteBuilderT0() : key_(name()) {
        DEBUG_BUILDER("ConcreteBuilderT0() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    explicit ConcreteBuilderT0(const typename base_t::key_t& k) : key_(k) {
        DEBUG_BUILDER("ConcreteBuilderT0() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    ConcreteBuilderT0(const ConcreteBuilderT0&) = delete;
    ConcreteBuilderT0(ConcreteBuilderT0&&)      = delete;

    // -- Destructor

    ~ConcreteBuilderT0() override {
        DEBUG_BUILDER("~ConcreteBuilderT0() -- " << T::className());
#if eckit_HAVE_ECKIT_MEMORY_FACTORY_EMPTY_DESTRUCTION
        Factory<typename base_t::product_t>::instance().unregist(key_);
#endif
    }

    // -- Operators

    void operator=(const ConcreteBuilderT0&) = delete;
    void operator=(ConcreteBuilderT0&&)      = delete;

    // -- Overridden methods

    typename base_t::key_t name() const override { return T::className(); }
    typename base_t::product_t* create() const override { return new T(); }

private:

    // -- Members

    typename base_t::key_t key_;
};


#define register_BuilderT0(ABSTRACT, CONCRETE, NAME)                           \
    static struct Register__##ABSTRACT##__##CONCRETE##__T0 {                   \
        Register__##ABSTRACT##__##CONCRETE##__T0() {                           \
            static eckit::ConcreteBuilderT0<ABSTRACT, CONCRETE> builder(NAME); \
        }                                                                      \
    } register_##ABSTRACT##__##CONCRETE##_T0


//------------------------------------------------------------------------------------------------------

template <class Base, class T>
class ConcreteBuilderT1 final : public BuilderT1<Base> {
public:

    // -- Types

    using base_t = BuilderT1<Base>;

    // -- Constructors

    ConcreteBuilderT1() : key_(name()) {
        DEBUG_BUILDER("ConcreteBuilderT1() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    explicit ConcreteBuilderT1(const typename base_t::key_t& k) : key_(k) {
        DEBUG_BUILDER("ConcreteBuilderT1() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    ConcreteBuilderT1(const ConcreteBuilderT1&) = delete;
    ConcreteBuilderT1(ConcreteBuilderT1&&)      = delete;

    // -- Destructor

    ~ConcreteBuilderT1() override {
        DEBUG_BUILDER("~ConcreteBuilderT1() -- " << T::className());
#if ECKIT_MEMORY_FACTORY_EMPTY_DESTRUCTION
        Factory<typename base_t::product_t>::instance().unregist(key_);
#endif
    }

    // -- Operators

    void operator=(const ConcreteBuilderT1&) = delete;
    void operator=(ConcreteBuilderT1&&)      = delete;

    // -- Overridden methods

    typename base_t::key_t name() const override { return T::className(); }
    typename base_t::product_t* create(typename base_t::ARG1 p1) const override { return new T(p1); }


private:

    // -- Members

    typename base_t::key_t key_;
};


#define register_BuilderT1(ABSTRACT, CONCRETE, NAME)                           \
    static struct Register__##ABSTRACT##__##CONCRETE##__T1 {                   \
        Register__##ABSTRACT##__##CONCRETE##__T1() {                           \
            static eckit::ConcreteBuilderT1<ABSTRACT, CONCRETE> builder(NAME); \
        }                                                                      \
    } register_##ABSTRACT##__##CONCRETE##_T1


//------------------------------------------------------------------------------------------------------

template <class Base, class T>
class ConcreteBuilderT2 final : public BuilderT2<Base> {
public:

    // -- Types

    using base_t = BuilderT2<Base>;

    // -- Constructors

    ConcreteBuilderT2() : key_(name()) {
        DEBUG_BUILDER("ConcreteBuilderT2() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    explicit ConcreteBuilderT2(const typename base_t::key_t& k) : key_(k) {
        DEBUG_BUILDER("ConcreteBuilderT2() -- " << T::className());
        Factory<typename base_t::product_t>::instance().regist(key_, this);
    }

    ConcreteBuilderT2(const ConcreteBuilderT2&) = delete;
    ConcreteBuilderT2(ConcreteBuilderT2&&)      = delete;

    // -- Destructor

    ~ConcreteBuilderT2() override {
        DEBUG_BUILDER("~ConcreteBuilderT2() -- " << T::className());
#if eckit_HAVE_ECKIT_MEMORY_FACTORY_EMPTY_DESTRUCTION
        Factory<typename base_t::product_t>::instance().unregist(key_);
#endif
    }

    // -- Operators

    void operator=(const ConcreteBuilderT2&) = delete;
    void operator=(ConcreteBuilderT2&&)      = delete;

    // -- Overridden methods

    typename base_t::key_t name() const override { return T::className(); }
    typename base_t::product_t* create(typename base_t::ARG1 p1, typename base_t::ARG2 p2) const override {
        return new T(p1, p2);
    }


private:

    // -- Members

    typename base_t::key_t key_;
};


#define register_BuilderT2(ABSTRACT, CONCRETE, NAME)                           \
    static struct Register__##ABSTRACT##__##CONCRETE##__T2 {                   \
        Register__##ABSTRACT##__##CONCRETE##__T2() {                           \
            static eckit::ConcreteBuilderT2<ABSTRACT, CONCRETE> builder(NAME); \
        }                                                                      \
    } register_##ABSTRACT##__##CONCRETE##_T2

//------------------------------------------------------------------------------------------------------

}  // namespace eckit

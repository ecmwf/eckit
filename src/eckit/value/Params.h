/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date July 2014

#ifndef eckit_value_Params_H
#define eckit_value_Params_H

#include <list>

#include "eckit/config/Parametrisation.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/value/Value.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

/// Params provides a value-based behaviour for parametrisations of objects.

/// A class `MyParams` to be wrapped into Params needs to implement:
///
/// 1. A constructor from `Stream&`
///
/// 2. A static method `className` returning a `const char*`
///
/// 3. A `get` function taking a `Params::key_t` and returning a `Params::value_t`:
///
///        Params::value_t get( const MyParams&, const Params::key_t& )
///
///    This function performs the lookup of a parameter by key.
///
/// 4. A `print` function taking a `std::ostream&`:
///
///        void print( const MyParams&, std::ostream& )
///
///   This function prints a representation of the parameters to the ostream.
///
/// 5. An `encode` function taking a `Stream&`:
///
///        void encode( const MyParams&, Stream& )
///
///    This function encodes the parameters to the Stream.
///
/// If the free functions `get`, `print` and `encode` need access to private
/// members of `MyParams`, they need to be declared as friend functions.
///
/// In addition a `Params::Factory<MyParams>` needs to be initialised.

class Params {

    struct Concept;

public:  // types

    typedef std::list<Params> List;
    typedef std::string key_t;
    typedef Value value_t;

    struct BaseFactory {
        virtual ~BaseFactory() {}
        virtual Concept* build(Stream& s) = 0;
    };

    typedef BaseFactory* factory_t;

    template <typename T>
    struct Factory : BaseFactory {
        Factory() { Params::registerFactory(T::className(), this); }

        Concept* build(Stream& s);
    };

public:  // methods

    template <typename T>
    explicit Params(const T& x) : self_(new Model<T>(x)) {}

    Params(const Params& x) : self_(x.self_->copy_()) {}

    static void registerFactory(const std::string& name, factory_t f) { factories()[name] = f; }

    static factory_t& getFactory(const std::string& name) { return factories()[name]; }

    static Params build(const std::string& name, Stream& s) {
        return Params(getFactory(name)->build(s));  // returns Concept*
    }

    static Params decode(Stream& s) {
        std::string name;
        s >> name;
        return build(name, s);
    }

    ~Params() { delete self_; }

    Params& operator=(Params x) {
        std::swap(x.self_, this->self_);
        return *this;
    }

    bool has(const std::string& name) const;

    value_t operator[](const key_t& key) const;

    friend void print(const Params& p, std::ostream& s);

    friend void encode(const Params& p, Stream& s);

    friend value_t getValue(const Params& p, const key_t& key);

private:  // internal classes

    typedef std::map<std::string, factory_t> factory_map;
    static factory_map& factories();

    Params(Concept* _concept) : self_(_concept) {}

    struct Concept {
        virtual ~Concept() {}
        virtual Concept* copy_() const               = 0;
        virtual value_t get_(const key_t& key) const = 0;
        virtual void print_(std::ostream& s) const   = 0;
        virtual void encode_(Stream& s) const        = 0;
    };

    template <typename T>
    struct Model : Concept {
        Model(T x) : data_(x) {}
        Model(Stream& s) : data_(s) {}

        virtual Concept* copy_() const { return new Model(data_); }

        virtual value_t get_(const key_t& key) const { return getValue(data_, key); }

        virtual void print_(std::ostream& s) const { print(data_, s); }

        virtual void encode_(Stream& s) const {
            s << T::className();
            encode(data_, s);
        }

        T data_;
    };

private:  // methods

    friend std::ostream& operator<<(std::ostream& s, const Params& p);

    friend Stream& operator<<(Stream& s, const Params& p);

private:  // members

    const Concept* self_;
};

//----------------------------------------------------------------------------

template <typename T>
Params::Concept* Params::Factory<T>::build(Stream& s) {
    return new Model<T>(s);
}

//----------------------------------------------------------------------------

}  // namespace eckit

#endif

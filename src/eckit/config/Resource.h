/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#pragma once

#include <string>

#include "eckit/config/Configuration.h"
#include "eckit/config/ResourceBase.h"
#include "eckit/value/Value.h"

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

template <class T>
class Resource : public ResourceBase {
public:  // methods

    // Standalone

    Resource(const std::string& str, const T& value) : ResourceBase(nullptr, str), value_(value) {}

    // Part of a configurable

    Resource(Configurable* owner, const std::string& str, const T& value) : ResourceBase(owner, str), value_(value) {}

    Resource(const std::string& str, const std::string& value, bool) :
        ResourceBase(nullptr, str), value_(eckit::Translator<std::string, T>()(value)) {}

    // -- Convertors

    operator const T&() const {
        const_cast<Resource<T>*>(this)->init();
        return value_;
    }

private:  // members

    T value_;

    // From ResourceBase

    virtual void setValue(const std::string& s) { value_ = Translator<std::string, T>()(s); }

    virtual std::string getValue() const { return Translator<T, std::string>()(value_); }
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Resource<T>& r) {
    os << static_cast<const T&>(r);
    return os;
}

//----------------------------------------------------------------------------------------------------------------------


template <class T, class LIB>
class LibResource : public ResourceBase {
public:  // methods

    LibResource(const std::string& str, const T& value) : ResourceBase(nullptr, str), value_(value) {}

    operator const T&() const {
        const_cast<LibResource<T, LIB>*>(this)->init();
        return value_;
    }

    friend std::ostream& operator<<(std::ostream& os, const LibResource<T, LIB>& r) {
        os << static_cast<const T&>(r);
        return os;
    }

private:  // members

    T value_;

    // From ResourceBase

    virtual void setValue(const std::string& s) { value_ = Translator<std::string, T>()(s); }

    virtual std::string getValue() const { return Translator<T, std::string>()(value_); }

    virtual bool setFromConfigFile() {
        std::string value;

        if (LIB::instance().configuration().get(name(), value)) {
            setValue(value);
            return true;
        }
        return false;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

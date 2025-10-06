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
/// @date March 2015

#ifndef eckit_value_DispatchParams_H
#define eckit_value_DispatchParams_H

#include "eckit/value/Params.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

template <class Derived>
class DispatchParams {

public:  // methods

    DispatchParams() {}
    DispatchParams(Stream& s) { NOTIMP; }

    static const char* className() { return "eckit::DispatchParams"; }

    template <typename T>
    friend Params::value_t getValue(const DispatchParams<T>& p, const Params::key_t& key);
    template <typename T>
    friend void print(const DispatchParams<T>&, std::ostream&);
    template <typename T>
    friend void encode(const DispatchParams<T>&, Stream&);

protected:  // members

    typedef Params::value_t (Derived::*parametrizer_t)(const Params::key_t&) const;
    typedef std::map<std::string, parametrizer_t> store_t;

    store_t dispatch_;
};

template <class Derived>
Params::value_t getValue(const DispatchParams<Derived>& p, const Params::key_t& key) {
    typename DispatchParams<Derived>::store_t::const_iterator i = p.dispatch_.find(key);
    if (i != p.dispatch_.end()) {
        typename DispatchParams<Derived>::parametrizer_t fptr = i->second;
        const Derived* pobj                                   = static_cast<const Derived*>(&p);
        return (pobj->*fptr)(key);
    }
    return Params::value_t();
}

template <class Derived>
void print(const DispatchParams<Derived>&, std::ostream&) {
    NOTIMP;
}

template <class Derived>
void encode(const DispatchParams<Derived>&, Stream&) {
    NOTIMP;
}

//----------------------------------------------------------------------------

}  // namespace eckit

#endif

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    using parametrizer_t = Params::value_t (Derived::*)(const Params::key_t&) const;
    using store_t        = std::map<std::string, parametrizer_t>;

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

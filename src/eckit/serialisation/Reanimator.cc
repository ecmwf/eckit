// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/serialisation/Streamable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
Reanimator<T>::Reanimator() : ReanimatorBase(T::classSpec()) {}

template <class T>
Streamable* Reanimator<T>::ressucitate(Stream& s) const {
    return new T(s);
}


template <class T>
T* Reanimator<T>::reanimate(Stream& s) {
    return static_cast<T*>(ReanimatorBase::reanimate(s, &T::classSpec()));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_Inited_h
#define eckit_Inited_h

#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// Serves to initialise to 0 numerical or pointer values (which
// should be removed when the compiler supports it)
// i.e. long() gets the value which is in the stack

template <class T>
class Inited {
public:

    Inited() : value_(0) {}
    Inited(const T& v) : value_(v) {}

    ~Inited() {}

    operator T&() { return value_; }
    operator const T&() const { return value_; }

    Inited<T>& operator+=(const Inited<T>& other);
    Inited<T>& operator-=(const Inited<T>& other);

private:

    T value_;
};

template <>
inline Inited<Ordinal>& Inited<Ordinal>::operator+=(const Inited<Ordinal>& other) {
    value_ += other.value_;
    return *this;
}

template <>
inline Inited<Ordinal>& Inited<Ordinal>::operator-=(const Inited<Ordinal>& other) {
    value_ -= other.value_;
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

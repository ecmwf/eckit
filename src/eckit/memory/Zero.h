// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

///@author Baudouin Raoult
///@author Tiago Quintino
///@date   Dec 2018

#ifndef eckit_memory_Zero_h
#define eckit_memory_Zero_h


#include <cstring>  // for memset


namespace eckit {

template <class T>
inline void zero(T& p) {
    ::memset(&p, 0, sizeof(T));
}

}  // namespace eckit

#endif

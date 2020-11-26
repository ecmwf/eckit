/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

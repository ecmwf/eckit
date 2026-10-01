// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_option_Title_H
#define eckit_option_Title_H

#include <string>

#include "eckit/filesystem/PathName.h"

namespace eckit::option {

namespace implementation_detail {

/*
 * The following is a utility used to automatically determine the description of an option type.
 */

template <class T>
struct Title {
    const char* operator()() const;
};

template <>
inline const char* Title<size_t>::operator()() const {
    return "ordinal";
}

template <>
inline const char* Title<long>::operator()() const {
    return "integer";
}

template <>
inline const char* Title<double>::operator()() const {
    return "real";
}

template <>
inline const char* Title<bool>::operator()() const {
    return "0/1";
}

template <>
inline const char* Title<std::string>::operator()() const {
    return "string";
}

template <>
inline const char* Title<eckit::PathName>::operator()() const {
    return "path";
}

}  // namespace implementation_detail

}  // namespace eckit::option

#endif

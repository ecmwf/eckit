// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <typeinfo>

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

std::string demangle(const char*);

template <typename T>
std::string demangle() {
    return demangle(typeid(T).name());
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec

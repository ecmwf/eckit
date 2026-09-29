// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/codec/detail/demangle.h"

#include "eckit/eckit_config.h"

#if eckit_HAVE_CXXABI_H
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#endif

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

std::string demangle(const char* mangled_name) {
#if eckit_HAVE_CXXABI_H
    int status = -4;

    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status), std::free};

    return status == 0 ? res.get() : mangled_name;
#else
    return {mangled_name};
#endif
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec

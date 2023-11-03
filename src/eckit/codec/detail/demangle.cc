/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/detail/demangle.h"

#include "eckit/eckit_config.h"

#if eckit_HAVE_CXXABI_H
#include <cxxabi.h>
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

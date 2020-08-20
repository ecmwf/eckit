/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <cstring>
#include <sstream>

#include "eckit/eckit.h"

#if defined(eckit_HAVE_EXECINFO_BACKTRACE) || defined(__FreeBSD__)
#include <execinfo.h>  // for backtrace
#endif

#ifdef eckit_HAVE_CXXABI_H
#include <cxxabi.h>
#endif

#include "eckit/exception/Exceptions.h"
#include "eckit/os/BackTrace.h"
#include "eckit/types/Types.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

std::string BackTrace::dump() {
    /// @todo implement this using the cxxabi demangle, if CMake detects it

    std::ostringstream oss;

#if (defined(eckit_HAVE_EXECINFO_BACKTRACE) || defined(__FreeBSD__)) && !defined(_AIX)

    static Ordinal count = 0;
    ++count;

#define BS_BUFF_SIZE 256

    void* buffer[BS_BUFF_SIZE];
    char** strings;

    int addsize = backtrace(buffer, BS_BUFF_SIZE);

    oss << "backtrace [" << count << "] stack has " << addsize << " addresses\n";

    strings = backtrace_symbols(buffer, addsize);
    if (strings == nullptr)
        oss << " --- no backtrace_symbols found ---\n";

#ifndef eckit_HAVE_CXXABI_H
    for (int s = 0; s < addsize; ++s)
        oss << strings[s] << std::endl;
#else
    for (int s = 0; s < addsize; ++s) {
        int status;
        char buffer[10240];
        bool overflow = false;

        char* p  = strings[s];
        size_t i = 0;
        while (*p) {
            switch (*p) {
                case ' ':
                case '(':
                case ')':
                case '+':
                case '\t':
                    oss << *p;
                    if (i) {
                        buffer[i++] = 0;
                        char* d     = abi::__cxa_demangle(buffer, nullptr, nullptr, &status);
                        if (status == 0) {
                            oss << d;
                        }
                        else {
                            oss << buffer;
                        }
                        if (d)
                            free(d);
                    }
                    i = 0;
                    break;

                default:
                    if (overflow) {
                        oss << *p;
                    }
                    else {
                        if (i < sizeof(buffer)) {
                            buffer[i++] = *p;
                        }
                        else {
                            overflow = true;
                            for (size_t j = 0; j < i; j++) {
                                oss << buffer[j];
                            }
                            i = 0;
                        }
                    }
            }

            p++;
        }
        oss << '\n';
    }
#endif

    free(strings);

    oss << "\nend of backtrace dump ...";

#else
    oss << "\ndumping backtrace not supported on this system";
#endif

    return oss.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

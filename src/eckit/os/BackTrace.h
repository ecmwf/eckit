// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_os_BackTrace_h
#define eckit_os_BackTrace_h

#include <string>

namespace eckit {

//--------------------------------------------------------------------------------------------------

class BackTrace {
public:

    static std::string dump();
};

//--------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

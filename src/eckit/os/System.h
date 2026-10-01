// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_os_System_h
#define eckit_os_System_h

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class System {
public:

    static std::string addrToPath(const void* addr);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

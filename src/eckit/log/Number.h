// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Aug 2018

#ifndef eckit_log_Number_H
#define eckit_log_Number_H

#include <iosfwd>
#include <string>

namespace eckit::log {

//----------------------------------------------------------------------------------------------------------------------

class Number {
public:

    using W = long long;

    static void printBinary(std::ostream&, W);
    static std::string printBinary(W);

    static void printHexadecimal(std::ostream&, W);
    static std::string printHexadecimal(W);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::log

#endif

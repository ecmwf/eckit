/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    typedef long long W;

    static void printBinary(std::ostream&, W);
    static std::string printBinary(W);

    static void printHexadecimal(std::ostream&, W);
    static std::string printHexadecimal(W);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::log

#endif

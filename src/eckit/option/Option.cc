// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#include "eckit/option/Option.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::option {

Option::Option(const std::string& name, const std::string& description) : name_(name), description_(description) {}

std::ostream& operator<<(std::ostream& s, const Option& p) {
    p.print(s);
    return s;
}

}  // namespace eckit::option

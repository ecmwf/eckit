// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <sstream>

#include "eckit/log/CodeLocation.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

std::string CodeLocation::asString() const {
    std::ostringstream oss;
    print(oss);
    return oss.str();
}

eckit::CodeLocation::operator std::string() const {
    return asString();
}

CodeLocation::operator bool() const {
    return file_ && ::strlen(file_);
}

void CodeLocation::print(std::ostream& os) const {
    if (file_) {
        os << " (" << file_ << ":" << line_;
        if (func_ && ::strlen(func_) > 0) {
            os << " " << func_;
        }
        os << ")";
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

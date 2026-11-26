// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/types/VerifyingDate.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

VerifyingDate::VerifyingDate(const Date& d, const Time& t) : DateTime(d, t) {
    ASSERT(Second(time_) == 0);
}

VerifyingDate::VerifyingDate(const std::string& s) : DateTime(s) {
    ASSERT(Second(time_) == 0);
}


VerifyingDate::VerifyingDate(time_t thetime) : DateTime(thetime) {
    ASSERT(Second(time_) == 0);
}

VerifyingDate::VerifyingDate(const DateTime& thetime) : DateTime(thetime) {
    ASSERT(Second(time_) == 0);
}

VerifyingDate::operator std::string() const {
    return std::string(date_);
}

void VerifyingDate::print(std::ostream& s) const {
    s << date_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

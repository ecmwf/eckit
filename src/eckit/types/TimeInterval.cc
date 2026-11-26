// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/types/TimeInterval.h"
#include "eckit/types/DateTime.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TimeInterval::TimeInterval(const DateTime& t1, const DateTime& t2) : begin_(t1), end_(t2) {}

void TimeInterval::print(std::ostream& s) const {
    s << "[" << begin_ << "," << end_ << "]";
}

TimeInterval TimeInterval::intersect(const TimeInterval& other) const {
    return TimeInterval(std::max(begin_, other.begin_), std::min(end_, other.end_));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

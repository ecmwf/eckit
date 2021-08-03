/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/types/TimeInterval.h"
#include "eckit/types/DateTime.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TimeInterval::TimeInterval(const DateTime& t1, const DateTime& t2) :
    begin_(t1), end_(t2) {}

void TimeInterval::print(std::ostream& s) const {
    s << "[" << begin_ << "," << end_ << "]";
}

TimeInterval TimeInterval::intersect(const TimeInterval& other) const {
    return TimeInterval(std::max(begin_, other.begin_), std::min(end_, other.end_));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

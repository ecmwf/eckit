/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/value/TimeContent.h"
#include "eckit/log/JSON.h"
#include "eckit/utils/Hash.h"
#include "eckit/value/NumberContent.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec TimeContent::classSpec_ = {
    &Content::classSpec(),
    "TimeContent",
};
Reanimator<TimeContent> TimeContent::reanimator_;

TimeContent::TimeContent(const Time& d) :
    value_(d) {}

TimeContent::TimeContent(Stream& s) :
    Content(s) {
    std::string dd;
    s >> dd;
    value_ = Time(dd);
}

void TimeContent::encode(Stream& s) const {
    Content::encode(s);
    std::string dd = value_;
    s << dd;
}

TimeContent::~TimeContent() {}

Content* TimeContent::clone() const {
    return new TimeContent(value_);
}

void TimeContent::print(std::ostream& s) const {
    s << value_;
}

void TimeContent::json(JSON& s) const {
    s << std::string(value_);
}

int TimeContent::compare(const Content& other) const {
    return -other.compareTime(*this);
}

int TimeContent::compareTime(const TimeContent& other) const {
    if (value_ == other.value_) {
        return 0;
    }

    return (value_ < other.value_) ? -1 : 1;
}

void TimeContent::value(Time& d) const {
    d = value_;
}

Content* TimeContent::add(const Content& other) const {
    return other.addTime(*this);
}

Content* TimeContent::sub(const Content& other) const {
    return other.subTime(*this);
}

Content* TimeContent::mul(const Content& other) const {
    return other.mulTime(*this);
}

Content* TimeContent::div(const Content& other) const {
    return other.divTime(*this);
}

Content* TimeContent::mod(const Content& other) const {
    return other.modTime(*this);
}

void TimeContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }

    out << "time(" << value_ << ")";
}

void TimeContent::hash(Hash& h) const {
    value_.hash(h);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

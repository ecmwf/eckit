/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/value/DateContent.h"
#include "eckit/log/JSON.h"
#include "eckit/utils/Hash.h"
#include "eckit/value/NumberContent.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec DateContent::classSpec_ = {
    &Content::classSpec(),
    "DateContent",
};
Reanimator<DateContent> DateContent::reanimator_;

DateContent::DateContent(const Date& d) :
    value_(d) {}

DateContent::DateContent(Stream& s) :
    Content(s) {
    std::string dd;
    s >> dd;
    value_ = Date(dd);
}

void DateContent::encode(Stream& s) const {
    Content::encode(s);
    std::string dd = value_;
    s << dd;
}

DateContent::~DateContent() {}

Content* DateContent::clone() const {
    return new DateContent(value_);
}

void DateContent::print(std::ostream& s) const {
    s << value_;
}

void DateContent::json(JSON& s) const {
    s << std::string(value_);
}

int DateContent::compare(const Content& other) const {
    return -other.compareDate(*this);
}

int DateContent::compareDate(const DateContent& other) const {
    if (value_ == other.value_)
        return 0;

    return (value_ < other.value_) ? -1 : 1;
}

void DateContent::value(Date& d) const {
    d = value_;
}

Content* DateContent::add(const Content& other) const {
    return other.addDate(*this);
}

Content* DateContent::sub(const Content& other) const {
    return other.subDate(*this);
}

Content* DateContent::subDate(const DateContent& other) const {
    return new NumberContent(value_ - other.value_);
}

Content* DateContent::mul(const Content& other) const {
    return other.mulDate(*this);
}

Content* DateContent::div(const Content& other) const {
    return other.divDate(*this);
}

Content* DateContent::mod(const Content& other) const {
    return other.modDate(*this);
}


void DateContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }
    out << "date(" << value_ << ")";
}

void DateContent::hash(Hash& h) const {
    value_.hash(h);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

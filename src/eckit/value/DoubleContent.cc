/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <limits>

#include "eckit/log/JSON.h"
#include "eckit/maths/Functions.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/DoubleContent.h"
#include "eckit/value/NumberContent.h"

#include "eckit/utils/Hash.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec DoubleContent::classSpec_ = {
    &Content::classSpec(),
    "DoubleContent",
};
Reanimator<DoubleContent> DoubleContent::reanimator_;

DoubleContent::DoubleContent(double l) :
    value_(l) {}

DoubleContent::DoubleContent(Stream& s) :
    Content(s), value_(0) {
    s >> value_;
}

Content* DoubleContent::clone() const {
    return new DoubleContent(value_);
}

void DoubleContent::encode(Stream& s) const {
    Content::encode(s);
    s << value_;
}

DoubleContent::~DoubleContent() {}

void DoubleContent::print(std::ostream& s) const {
    s << value_;
}

void DoubleContent::json(JSON& s) const {
    s << value_;
}


int DoubleContent::compare(const Content& other) const {
    return -other.compareDouble(*this);
}

int DoubleContent::compareDouble(const DoubleContent& other) const {
    double diff = (value_ - other.value_);

    return eckit::sign(diff);
}

int DoubleContent::compareNumber(const NumberContent& other) const {
    double diff = (value_ - other.value_);

    return eckit::sign(diff);
}

void DoubleContent::value(double& l) const {
    l = value_;
}

void DoubleContent::value(long long& l) const {
    // Avoid FE_INVALID by checking if value_ is within valid range of long long
    constexpr double min = double(std::numeric_limits<long long>::min());
    constexpr double max = double(std::numeric_limits<long long>::max());
    if (value_ <= min || value_ >= max) {
        Content::value(l);  // throws BadConversion
        return;
    }
    // Now safe to assign without FE_INVALID
    l = value_;
    if (l != value_) {
        Content::value(l);  // throws BadConversion
    }
}

void DoubleContent::value(std::string& s) const {
    s = Translator<double, std::string>()(value_);
}

Content* DoubleContent::add(const Content& other) const {
    return other.addDouble(*this);
}

Content* DoubleContent::addDouble(const DoubleContent& other) const {
    return new DoubleContent(other.value_ + value_);
}

Content* DoubleContent::sub(const Content& other) const {
    return other.subDouble(*this);
}

Content* DoubleContent::subDouble(const DoubleContent& other) const {
    return new DoubleContent(other.value_ - value_);
}

Content* DoubleContent::mul(const Content& other) const {
    return other.mulDouble(*this);
}

Content* DoubleContent::mulDouble(const DoubleContent& other) const {
    return new DoubleContent(other.value_ * value_);
}

Content* DoubleContent::div(const Content& other) const {
    return other.divDouble(*this);
}

Content* DoubleContent::mod(const Content& other) const {
    return other.modDouble(*this);
}


Content* DoubleContent::divDouble(const DoubleContent& other) const {
    return new DoubleContent(other.value_ / value_);
}

Value DoubleContent::negate() const {
    return Value(-value_);
}

void DoubleContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }
    out << "double(" << value_ << ")";
}

void DoubleContent::hash(Hash& h) const {
    h.add(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

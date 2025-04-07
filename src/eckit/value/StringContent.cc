/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <ostream>

#include "eckit/log/JSON.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/StringContent.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


ClassSpec StringContent::classSpec_ = {
    &Content::classSpec(),
    "StringContent",
};
Reanimator<StringContent> StringContent::reanimator_;


StringContent::StringContent(const std::string& s) : value_(s) {}

StringContent::StringContent(const char* s) : value_(s) {}

StringContent::StringContent(Stream& s) : Content(s) {
    s >> value_;
}

Content* StringContent::clone() const {
    return new StringContent(value_);
}


void StringContent::encode(Stream& s) const {
    Content::encode(s);
    s << value_;
}

StringContent::~StringContent() {}

void StringContent::print(std::ostream& s) const {
    s << value_;
}

void StringContent::json(JSON& s) const {
    s << value_;
}

int StringContent::compare(const Content& other) const {
    return -other.compareString(*this);
}

int StringContent::compareString(const StringContent& other) const {
    return value_.compare(other.value_);
}

void StringContent::value(std::string& s) const {
    s = value_;
}

void StringContent::value(bool& b) const {
    std::string v = StringTools::lower(value_);
    if (v == "true" || v == "on" || v == "yes" || v == "1") {
        b = true;
    }
    else if (v == "false" || v == "off" || v == "no" || v == "0") {
        b = false;
    }
    else {
        Content::value(b);
    }
}

void StringContent::value(long long& l) const {
    l = Translator<std::string, long long>()(value_);
}

void StringContent::value(double& d) const {
    d = Translator<std::string, double>()(value_);
}

Content* StringContent::add(const Content& other) const {
    return other.addString(*this);
}

Content* StringContent::addString(const StringContent& other) const {
    return new StringContent(other.value_ + value_);
}

Content* StringContent::sub(const Content& other) const {
    return other.subString(*this);
}

Content* StringContent::mul(const Content& other) const {
    return other.mulString(*this);
}

Content* StringContent::div(const Content& other) const {
    return other.divString(*this);
}

Content* StringContent::mod(const Content& other) const {
    return other.modString(*this);
}

void StringContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }
    // out << "string(" << value_ << ")";
    out << '"' << value_ << '"';
}

void StringContent::hash(Hash& h) const {
    h.add(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

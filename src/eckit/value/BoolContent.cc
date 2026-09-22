// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/value/BoolContent.h"

#include "eckit/log/JSON.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Translator.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec BoolContent::classSpec_ = {
    &Content::classSpec(),
    "BoolContent",
};
Reanimator<BoolContent> BoolContent::reanimator_;

BoolContent::BoolContent(bool l) : value_(l) {}

BoolContent::BoolContent(Stream& s) : Content(s), value_(false) {
    s >> value_;
}

Content* BoolContent::clone() const {
    return new BoolContent(value_);
}

void BoolContent::encode(Stream& s) const {
    Content::encode(s);
    s << value_;
}

BoolContent::~BoolContent() {}

void BoolContent::print(std::ostream& s) const {
    s << (value_ ? "true" : "false");
}

void BoolContent::json(JSON& s) const {
    s << value_;
}

int BoolContent::compare(const Content& other) const {
    return -other.compareBool(*this);
}

int BoolContent::compareBool(const BoolContent& other) const {
    bool equal = !(value_ - other.value_);

    if (equal) {
        return 0;  // both equal in value, hence 0
    }

    if (!value_) {
        return -1;  // this is false, hence smaller than other
    }

    return 1;  // this is true, hence larger than other
}

void BoolContent::value(bool& l) const {
    l = value_;
}

void BoolContent::value(std::string& s) const {
    s = value_ ? "true" : "false";
}

void BoolContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }

    out << (value_ ? "true" : "false");
}

void BoolContent::value(long long& l) const {
    l = value_;
}

void BoolContent::value(double& d) const {
    d = value_;
}

Content* BoolContent::add(const Content& other) const {
    return other.addBool(*this);
}


Content* BoolContent::sub(const Content& other) const {
    return other.subBool(*this);
}

Content* BoolContent::mul(const Content& other) const {
    return other.mulBool(*this);
}

Content* BoolContent::div(const Content& other) const {
    return other.divBool(*this);
}

Content* BoolContent::mod(const Content& other) const {
    return other.modBool(*this);
}

void BoolContent::hash(Hash& h) const {
    h.add(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

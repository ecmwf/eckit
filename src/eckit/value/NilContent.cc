// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/value/NilContent.h"
#include "eckit/log/JSON.h"
#include "eckit/value/Value.h"

#include "eckit/utils/Hash.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec NilContent::classSpec_ = {
    &Content::classSpec(),
    "NilContent",
};
Reanimator<NilContent> NilContent::reanimator_;


NilContent::NilContent() {}


NilContent::NilContent(Stream& s) : Content(s) {}

Content* NilContent::clone() const {
    return new NilContent();
}

void NilContent::encode(Stream& s) const {
    Content::encode(s);
}

NilContent::~NilContent() {}

void NilContent::value(ValueList& v) const {
    v = ValueList();
}

void NilContent::print(std::ostream& out) const {
    out << "(nil)";
}

void NilContent::json(JSON& s) const {
    s.null();
}

int NilContent::compare(const Content& other) const {
    return -other.compareNil(*this);
}

int NilContent::compareNil(const NilContent&) const {
    return 0;  // They're equals
}

Content* NilContent::add(const Content& other) const {
    return other.addNil(*this);
}

Content* NilContent::addNil(const NilContent&) const {
    return (Content*)this;
}

Content* NilContent::sub(const Content& other) const {
    return other.subNil(*this);
}

Content* NilContent::subNil(const NilContent&) const {
    return (Content*)this;
}

Content* NilContent::mul(const Content& other) const {
    return other.mulNil(*this);
}

Content* NilContent::mulNil(const NilContent&) const {
    return (Content*)this;
}

Content* NilContent::div(const Content& other) const {
    return other.divNil(*this);
}

Content* NilContent::divNil(const NilContent&) const {
    return (Content*)this;
}

Content* NilContent::mod(const Content& other) const {
    return other.divNil(*this);
}

bool NilContent::contains(const Value&) const {
    return false;
}

void NilContent::dump(std::ostream& out, size_t depth, bool indent) const {
    if (indent) {
        while (depth-- > 0) {
            out << ' ';
        }
    }
    out << "nil";
}

void NilContent::hash(Hash& h) const {
    h.add("");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

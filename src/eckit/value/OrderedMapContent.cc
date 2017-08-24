/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/value/OrderedMapContent.h"
#include "eckit/parser/JSON.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec OrderedMapContent::classSpec_ = {&Content::classSpec(), "OrderedMapContent",};
Reanimator<OrderedMapContent>  OrderedMapContent::reanimator_;

OrderedMapContent::OrderedMapContent() {

}

OrderedMapContent::OrderedMapContent(const ValueMap& v, const ValueList& keys):
    MapContent(v) {
    ASSERT(keys.size() == v.size());

    keys_ = keys;
}


OrderedMapContent::OrderedMapContent(Stream& s):
    MapContent(s)
{
    for (size_t i = 0; i < value_.size(); i++) {
        Value v(s);
        keys_.push_back(v);
    }
}

void OrderedMapContent::encode(Stream& s) const
{
    MapContent::encode(s);
    for (size_t i = 0; i < value_.size(); i++) {
        s << keys_[i];
    }
}

OrderedMapContent::~OrderedMapContent()
{
}

void OrderedMapContent::value(ValueMap& v) const
{
    NOTIMP;
}

Value OrderedMapContent::keys() const {
    return keys_;
}

void OrderedMapContent::print(std::ostream& s) const
{
    s << '{';
    for (ValueList::const_iterator j = keys_.begin(); j != keys_.end(); ++j)
    {
        if (j != keys_.begin()) s << " , ";
        s << *j;
        s << " => ";
        s << value(*j);
    }
    s << '}';
}

void OrderedMapContent::json(JSON& s) const
{
    s.startObject();
    for (ValueList::const_iterator j = keys_.begin(); j != keys_.end(); ++j)
    {
        s << *j;
        s << value(*j);
    }
    s.endObject();
}


Content* OrderedMapContent::clone() const {
    ValueMap v;
    for (ValueMap::const_iterator j = value_.begin(); j != value_.end(); ++j) {
        v[(*j).first.clone()] = (*j).second.clone();
    }
    return new OrderedMapContent(v, keys_);
}

void OrderedMapContent::dump(std::ostream& out, size_t depth, bool indent) const {

    if (indent) {
        size_t n = depth;
        while (n-- > 0) {
            out << ' ';
        }
    }

    out << "{";
    const char* sep = "\n";

    for (ValueList::const_iterator j = keys_.begin(); j != keys_.end(); ++j)
    {
        out << sep;
        (*j).dump(out, depth + 3);
        out << ": ";
        value(*j).dump(out, depth + 3, false);
        sep = ",\n";
    }

    if (!value_.empty()) {
        out << '\n';
        size_t n = depth;
        while (n-- > 0) {
            out << ' ';
        }
    }

    out << "}";
}

const Value& OrderedMapContent::value(const Value& key) const {
    ValueMap::const_iterator j = value_.find(key);
    ASSERT(j != value_.end());
    return (*j).second;
}


//-----------------------------------------------------------------------------

} // namespace eckit


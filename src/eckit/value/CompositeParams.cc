/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/value/CompositeParams.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

CompositeParams::CompositeParams() :
    plist_() {}

CompositeParams::CompositeParams(const Params::List& plist) :
    plist_(plist) {}

CompositeParams::CompositeParams(Stream& s) {
    Params::List::size_type len;
    s >> len;
    for (Params::List::size_type i = 0; i < len; ++i)
        push_back(Params::decode(s));
}

CompositeParams& CompositeParams::push_front(const Params& p) {
    plist_.push_front(p);
    return *this;
}

CompositeParams& CompositeParams::push_back(const Params& p) {
    plist_.push_back(p);
    return *this;
}

Params::value_t getValue(const CompositeParams& p, const Params::key_t& key) {
    for (Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr) {
        Value v = getValue(*citr, key);
        if (!v.isNil())
            return v;
    }
    return Value();
}

//----------------------------------------------------------------------------------------------------------------------

void print(const CompositeParams& p, std::ostream& s) {
    for (Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr)
        print(*citr, s);
}

void encode(const CompositeParams& p, Stream& s) {
    s << p.plist_.size();
    for (Params::List::const_iterator citr = p.plist_.begin(); citr != p.plist_.end(); ++citr)
        encode(*citr, s);
}

//----------------------------------------------------------------------------------------------------------------------

Params::Factory<CompositeParams> compositeParamsFactory;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

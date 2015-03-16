/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/value/ValueParams.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

ValueParams & ValueParams::set(const Params::key_t& k, const Params::value_t& v)
{
    props_.set(k,v);
    return *this;
}

Params::value_t get( const ValueParams& p, const Params::key_t& key )
{
    return p.props_.get(key); // returns Value Nil if doesn't exist
}

void print(const ValueParams & p, std::ostream &s)
{
    s << p.props_;
}

void encode(const ValueParams& p, Stream& s)
{
    s << p.props_;
}

Params::Factory<ValueParams> valueParamsFactory;

//----------------------------------------------------------------------------

} // namespace eckit

/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"

#include "eckit/xpr/Vector.h"
#include "eckit/xpr/LoadVector.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

LoadVector::LoadVector(size_t size)
    : Value(), size_(size)
{
}

LoadVector::LoadVector(Stream &s) : Value(s)
{
    s >> size_;
}

void LoadVector::encode(Stream &s) const
{
    Value::encode(s);
    s << size_;
}

ExpPtr LoadVector::cloneWith(args_t& a) const
{
    NOTIMP; // Should not be called
}

void LoadVector::print(std::ostream&o) const
{
    o << nodeName() << "(" << size_ << ")";
}

void LoadVector::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << size_ << ")";
}

void LoadVector::asJSON(JSON& s) const
{
    s.startObject();
    s << factoryName() << size_;
    s.endObject();
}

ExpPtr LoadVector::evaluate( Scope &ctx ) const
{
    // FIXME: using random initialisation for now
    return vector(size_,[](size_t){ return (real_t)std::rand()/RAND_MAX; });
}

//-----------------------------------------------------------------------------

ClassSpec LoadVector::classSpec_ = {
    &Value::classSpec(),
    LoadVector::nodeName(),
};

Reanimator< LoadVector > LoadVector::reanimator_;

//-----------------------------------------------------------------------------

ExpPtr loadVector( size_t size )
{
    return ExpPtr( new LoadVector(size) );
}

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

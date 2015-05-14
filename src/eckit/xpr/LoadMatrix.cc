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

#include "eckit/xpr/Matrix.h"
#include "eckit/xpr/LoadMatrix.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

LoadMatrix::LoadMatrix(size_t rows, size_t cols)
    : Value(), rows_(rows), cols_(cols)
{
}

LoadMatrix::LoadMatrix(Stream &s) : Value(s)
{
    s >> rows_;
    s >> cols_;
}

void LoadMatrix::encode(Stream &s) const
{
    Value::encode(s);
    s << rows_;
    s << cols_;
}

ExpPtr LoadMatrix::cloneWith(args_t& a) const
{
    NOTIMP; // Should not be called
}

void LoadMatrix::print(std::ostream&o) const
{
    o << nodeName() << "(" << rows_ << ", " << cols_ << ")";
}

void LoadMatrix::asCode(std::ostream&o) const
{
    o << factoryName() << "(" << rows_ << ", " << cols_ << ")";
}

void LoadMatrix::asJSON(JSON& s) const
{
    s.startObject();
    s << factoryName() << rows_ << cols_;
    s.endObject();
}

ExpPtr LoadMatrix::evaluate( Scope &ctx ) const
{
    // FIXME: using random initialisation for now
    return matrix(rows_, cols_, [](size_t, size_t){ return (real_t)std::rand()/RAND_MAX; });
}

//-----------------------------------------------------------------------------

ClassSpec LoadMatrix::classSpec_ = {
    &Value::classSpec(),
    LoadMatrix::nodeName(),
};

Reanimator< LoadMatrix > LoadMatrix::reanimator_;

//-----------------------------------------------------------------------------

ExpPtr loadMatrix( size_t rows, size_t cols )
{
    return ExpPtr( new LoadMatrix(rows, cols) );
}

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

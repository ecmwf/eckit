/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/FMap.h"
#include "eckit/maths/Scalar.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

FMap::FMap( ExpPtr f,  ExpPtr list ) : Func()
{
    args_.push_back(f);
    args_.push_back(list);
}

string FMap::ret_signature() const
{
    return Scalar::sig();
}

ValPtr FMap::evaluate()
{
    return maths::scalar( param(0)->arity() )->as<Value>();
}

//--------------------------------------------------------------------------------------------

ExpPtr fmap( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new FMap(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

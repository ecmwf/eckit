/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Reducer.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ExpPtr Reducer::apply(ExpPtr e)
{
    std::string signature = e->signature();

    reducers_t& reducer = reducers();
    std::map<std::string,Reducer*>::const_iterator itr = reducer.find(signature);
    if( itr == reducer.end() )
        return e;
    return (*itr).second->reduce(e);
}

Reducer::Reducer(const string &signature)
{
    reducers()[signature] = this;
}

Reducer::reducers_t& Reducer::reducers()
{
    static reducers_t reducers_;
    return reducers_;
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

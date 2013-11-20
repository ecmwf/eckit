/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ExpPtr Optimiser::apply(ExpPtr e)
{
    DBGX(*e);

    std::string signature = e->signature();

    optimisers_t& optimiser = optimisers();
    std::map<std::string,Optimiser*>::const_iterator itr = optimiser.find(signature);
    if( itr == optimiser.end() )
    {
        return e;
    }
    else
    {        
        ExpPtr re = (*itr).second->optimise(e);
        DBGX("... optimising ...");
        DBGX(*re);
        return re;
    }
}

Optimiser::Optimiser(const string &signature)
{
    optimisers()[signature] = this;
}

Optimiser::optimisers_t& Optimiser::optimisers()
{
    static optimisers_t optimisers_;
    return optimisers_;
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Optimiser.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Optimiser_h
#define eckit_maths_Optimiser_h

#include "eckit/maths/Expr.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Optimiser {
public:

    typedef std::map<std::string,Optimiser*> optimisers_t;

    static ExpPtr apply( ExpPtr e );

protected:

    static optimisers_t& optimisers();

    Optimiser( const std::string& signature );

    virtual ExpPtr optimise( ExpPtr ) const = 0;

};

//--------------------------------------------------------------------------------------------

/// Helper class that regists the reduction between 2 expressions
/// to be used when the optimised expression has a constructor fromt he target expression
template< typename T>
class OptimiseTo : public Optimiser {
public:

    OptimiseTo( const std::string& signature ) : Optimiser(signature) {}

private:

    /// Relies on proper contructor in the optimised expression
    ExpPtr optimise( ExpPtr e ) const
    {
        return ExpPtr( new T(e) );
    }

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif

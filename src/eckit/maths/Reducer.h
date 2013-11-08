/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Reducer.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Reducer_h
#define eckit_maths_Reducer_h

#include "eckit/maths/Exp.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class Reducer {
public:

    typedef std::map<std::string,Reducer*> reducers_t;

    static ExpPtr apply( ExpPtr e );

protected:

    static reducers_t& reducers();

    Reducer( const std::string& signature );

    virtual ExpPtr reduce( ExpPtr ) const = 0;

};

//--------------------------------------------------------------------------------------------

/// Helper class that regists the reduction between 2 expressions
/// to be used when the reduced expression has a constructor fromt he target expression
template< typename T>
class ReduceTo : public Reducer {
public:

    ReduceTo( const std::string& signature ) : Reducer(signature) {}

private:

    /// Relies on proper contructor in the reduced expression
    ExpPtr reduce( ExpPtr e ) const
    {
        return ExpPtr( new T(e) );
    }

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif

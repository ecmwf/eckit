/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Function.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Function_h
#define eckit_maths_Function_h

#include "eckit/maths/Expression.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

/// Represents a Function with some arguments
class Function : public Expression {

public: // types

    typedef std::string key_t;

    typedef boost::function< ValPtr ( const args_t& ) > func_t;
    typedef std::map< key_t, func_t > dispatcher_t;

    typedef boost::function< ExpPtr ( const args_t& ) > builder_t;
    typedef std::map< key_t, builder_t > factory_t;

protected: // methods

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Function();

    /// Contructor taking a list of parameters
    Function( const args_t& args );

public: // methods

    virtual ~Function();

    static dispatcher_t& dispatcher();
    static factory_t& factory();

    static std::string className() { return "Function"; }

public: // virtual methods

    virtual ValPtr evaluate( Scope& ctx );

    virtual ExpPtr optimise();

    virtual ExpPtr clone();

    virtual std::string signature() const;

    virtual void print( std::ostream& o ) const;

    template< typename T >
    struct RegisterFactory
    {
        RegisterFactory() { factory()[ T::className() ] = &(RegisterFactory<T>::build); }
        static ExpPtr build( const args_t& args )
        {
            return ExpPtr( new T( args ) );
        }
    };

protected: // methods

    std::string signatureArguments( const args_t& args ) const;

};

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif

/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_memory_Builder_h
#define eckit_memory_Builder_h

/// @file Builder.h
/// @author Tiago Quintino
/// @date Jul 2014

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/Factory.h"
#include "eckit/value/Params.h"

//-----------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

template< class Base >
class Builder {

public: // types

	typedef Base product_t;
	typedef product_t* product_ptr;
	typedef std::string key_t;
	typedef typename Factory<Base>::builder_ptr builder_ptr;

public: // methods

	virtual ~Builder() {}

	virtual key_t name() const = 0;

	virtual product_ptr create( const Params& ) const = 0;

	friend std::ostream& operator<<( std::ostream& os, const Builder<Base>& o) { o.print(os); return os;}

private: // methods

	virtual void print( std::ostream& os ) const { os << name(); }

};

//------------------------------------------------------------------------------------------------------

template< class Base, class T >
class ConcreteBuilder : public Builder<Base> {

public: // types

	typedef Base product_t;
	typedef product_t* product_ptr;
	typedef Builder<Base> base_t;
	typedef typename base_t::builder_ptr builder_ptr;

public: // methods

	ConcreteBuilder()
	{
		Factory<product_t>::instance().regist( builder_ptr(this) );
	}

	virtual ~ConcreteBuilder()
	{
		Factory<product_t>::instance().unregist( name() );
	}

	virtual typename base_t::key_t name() const { return T::className(); }

	virtual product_ptr create( const Params& p ) const { return new T(p); }

};


//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_memory_Builder_h

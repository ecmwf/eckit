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

class Builder : private NonCopyable {
public:

	typedef std::string key_t;

	virtual ~Builder() {}
	virtual key_t name() const = 0;
	virtual key_t build_type() const = 0;

	friend std::ostream& operator<<( std::ostream& os, const Builder& o) { o.print(os); return os;}

private: // methods

	virtual void print( std::ostream& os ) const { os << "Builder(" << build_type() << "):" << name(); }

};

//------------------------------------------------------------------------------------------------------

template< class Base >
class BuilderT0 : public Builder {

public: // types

	typedef Base product_t;
	typedef product_t* product_ptr;
	typedef Builder::key_t key_t;
	typedef typename Factory<Base>::builder_ptr builder_ptr;

	virtual product_ptr create() const  = 0;

public: // methods

	virtual key_t build_type() const { return Base::className(); }

};

//------------------------------------------------------------------------------------------------------

template< class Base >
class BuilderT1 : public Builder {

public: // types

	typedef Base product_t;
	typedef product_t* product_ptr;
	typedef Builder::key_t key_t;
	typedef typename Factory<Base>::builder_ptr builder_ptr;

	typedef typename product_t::ARG1 ARG1;

	virtual product_ptr create( ARG1 p1 ) const = 0;

public: // methods

	virtual key_t build_type() const { return Base::className(); }

};

//------------------------------------------------------------------------------------------------------

template< class Base >
class BuilderT2 : public Builder {

public: // types

	typedef Base product_t;
	typedef product_t* product_ptr;
	typedef Builder::key_t key_t;
	typedef typename Factory<Base>::builder_ptr builder_ptr;

	typedef typename product_t::ARG1 ARG1;
	typedef typename product_t::ARG2 ARG2;

	virtual product_ptr create( ARG1 p1, ARG2 p2 ) const = 0;

public: // methods

	virtual key_t build_type() const { return Base::className(); }

};

//------------------------------------------------------------------------------------------------------

template< class Base, class T >
class ConcreteBuilderT0 : public BuilderT0<Base> {

public: // types

	typedef BuilderT0<Base> base_t;

	typedef typename base_t::key_t key_t;
	typedef typename base_t::product_t product_t;
	typedef typename base_t::product_ptr product_ptr;
	typedef typename base_t::builder_ptr builder_ptr;

public: // methods

	ConcreteBuilderT0() : k_(name())
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this) );
	}

	ConcreteBuilderT0(const key_t& k) : k_(k)
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this) );
	}

	virtual ~ConcreteBuilderT0()
	{
		Factory<product_t>::instance().unregist( name() );
	}

	virtual typename base_t::key_t name() const { return T::className(); }

	virtual product_ptr create() const { return new T(); }

private:

	key_t k_;

};

//------------------------------------------------------------------------------------------------------

template< class Base, class T >
class ConcreteBuilderT1 : public BuilderT1<Base> {
public: // types

	typedef BuilderT1<Base> base_t;

	typedef typename base_t::key_t key_t;
	typedef typename base_t::product_t product_t;
	typedef typename base_t::product_ptr product_ptr;
	typedef typename base_t::builder_ptr builder_ptr;

	typedef typename base_t::ARG1 ARG1;

public: // methods

	ConcreteBuilderT1() : k_(name())
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this) );
	}

	ConcreteBuilderT1(const key_t& k) : k_(k)
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this) );
	}

	virtual ~ConcreteBuilderT1()
	{
		Factory<product_t>::instance().unregist( k_ );
	}

	virtual typename base_t::key_t name() const { return T::className(); }

	virtual product_ptr create( ARG1 p1 ) const { return new T(p1); }

private:

	key_t k_;

};

//------------------------------------------------------------------------------------------------------

template< class Base, class T >
class ConcreteBuilderT2 : public BuilderT2<Base> {

public: // types

	typedef BuilderT2<Base> base_t;

	typedef typename base_t::key_t key_t;
	typedef typename base_t::product_t product_t;
	typedef typename base_t::product_ptr product_ptr;
	typedef typename base_t::builder_ptr builder_ptr;

	typedef typename base_t::ARG1 ARG1;
	typedef typename base_t::ARG2 ARG2;

public: // methods

	ConcreteBuilderT2() : k_(name())
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this) );
	}

	ConcreteBuilderT2(const key_t& k) : k_(k)
	{
		Factory<product_t>::instance().regist(k_, builder_ptr(this));
	}

	virtual ~ConcreteBuilderT2()
	{
		Factory<product_t>::instance().unregist( name() );
	}

	virtual typename base_t::key_t name() const { return T::className(); }

	virtual product_ptr create( ARG1 p1, ARG2 p2 ) const { return new T(p1,p2); }

private:

	key_t k_;

};

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_memory_Builder_h

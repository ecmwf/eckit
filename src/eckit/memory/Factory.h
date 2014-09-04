/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_memory_Factory_h
#define eckit_memory_Factory_h

/// @file Factory.h
/// @author Tiago Quintino
/// @date Jul 2014

#include "eckit/memory/SharedPtr.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//------------------------------------------------------------------------------------------------------

template< class T >
class Factory {

public: // types

	typedef std::string key_t;

	typedef T product_t;
	typedef typename product_t::builder_t builder_t;
	typedef builder_t* builder_ptr;

	typedef std::map<key_t,builder_ptr> storage_t;

public: // methods

	/// @return the instance of this singleton factory
	static Factory<T>& instance();

	/// @returns class name of the type built by this factory
	static std::string build_type() { return T::className(); }

	/// Checks if a builder is registered
	/// @param name of the builder
	bool exists( const key_t& k) { return (store_.find(k) != store_.end() ); }

	/// Registers a builder
	/// @param builder pointer
	/// @throw BadParameter if the builder already registered
	void regist( const key_t&, builder_ptr );

	/// Remove a registered builder
	/// @throw BadParameter if the builder is not registered
	void unregist( const key_t& );

	/// Gets the builder registered to the associated key
	/// @param name of the builder
	const builder_t& get( const key_t& name );

	size_t size() const { return store_.size(); }

	friend std::ostream& operator<<( std::ostream& os, const Factory<T>& o) { o.print(os); return os;}

private: // methods

	void print( std::ostream& ) const;

	Factory()  {}

	~Factory() {}

private: // members

	mutable Mutex mutex_;
	storage_t store_; ///< storage for the builders in a map indexed by key_t

};

//------------------------------------------------------------------------------------------------------

template <class T>
Factory<T>& Factory<T>::instance()
{
	static Factory<T> obj;
	return obj;
}

template <class T>
void Factory<T>::regist( const key_t& k, builder_ptr b )
{
	AutoLock<Mutex> lock(mutex_);
	ASSERT( b );
	if( exists(k) )
	{
		throw BadParameter( "Factory of " + build_type() + " has already a builder for " + k, Here() );
	}
	store_[k] = b;
}

template <class T>
void Factory<T>::unregist(const key_t& k)
{
	AutoLock<Mutex> lock(mutex_);
	if( !exists(k) )
	{
		throw BadParameter( "Factory of " + build_type() + " has no builder for " + k, Here() );
	}
	store_.erase( k );
}

template <class T>
const typename Factory<T>::builder_t& Factory<T>::get(const key_t& k)
{
	AutoLock<Mutex> lock(mutex_);
	if( !exists(k) )
	{
		throw BadParameter( "Factory of " + build_type() + " has no builder for " + k, Here() );
	}
	return *store_[k];
}

template <class T>
void Factory<T>::print(std::ostream& os) const
{
	AutoLock<Mutex> lock(mutex_);
	os << "Factory(" << build_type() << ")" << std::endl;
	for( typename storage_t::const_iterator i = store_.begin(); i != store_.end(); ++i )
	{
		os << (*(*i).second) << std::endl;
	}
}

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_memory_Factory_h

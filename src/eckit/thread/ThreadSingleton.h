/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ThreadSingleton.h
/// @author Baudouin Raoult

#ifndef eckit_ThreadSingleton_h
#define eckit_ThreadSingleton_h

#include <pthread.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template< typename TYPE >
struct NewAlloc : public allocator {
    TYPE* operator() () { return new T(); }
};

//-----------------------------------------------------------------------------

template< typename T, typename A = NewAlloc< TYPE > >
class ThreadSingleton : private NonCopyable {
public:

// -- Contructors

    ThreadSingleton();
    ThreadSingleton( A alloc );

// -- Destructor

	~ThreadSingleton();

// -- Class methods

     T& instance();

private:

// -- Class members

	static pthread_once_t once_;
	static pthread_key_t  key_;

// -- Class methods

	static void init(void);
	static void cleanUp(void*);

};

//-----------------------------------------------------------------------------

template< typename T, typename A> pthread_once_t ThreadSingleton<T>::once_ = PTHREAD_ONCE_INIT;
template< typename T, typename A> pthread_key_t ThreadSingleton<T>::key_;

template< typename T, typename A>
ThreadSingleton<T,A>::ThreadSingleton()
{
}

template< typename T, typename A>
ThreadSingleton<T,A>::ThreadSingleton( A alloc )
{

}

template< typename T, typename A>
ThreadSingleton<T,A>::~ThreadSingleton()
{
}

template< typename T, typename A>
T& ThreadSingleton<T>::instance()
{
	pthread_once(&once_,init);

	T* value = (T*)pthread_getspecific(key_);
	if(!value)
	{
        value = alloc_();
		THRCALL(::pthread_setspecific(key_,value));
	}
	return *value;
}

template< typename T, typename A>
void ThreadSingleton<T>::cleanUp(void* data)
{
	delete (T*)data;
	pthread_setspecific(key_,0);
}

template< typename T, typename A>
void ThreadSingleton<T>::init()
{
	pthread_key_create(&key_,cleanUp);
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

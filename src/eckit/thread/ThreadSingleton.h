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

template<class T> class ThreadSingleton : private NonCopyable {
public:

// -- Contructors

	ThreadSingleton();

// -- Destructor

	~ThreadSingleton();

// -- Class methods

	static T& instance();

private:

// -- Class members

	static pthread_once_t once_;
	static pthread_key_t  key_;

// -- Class methods

	static void init(void);
	static void cleanUp(void*);

};

//-----------------------------------------------------------------------------

template<class T> pthread_once_t ThreadSingleton<T>::once_ = PTHREAD_ONCE_INIT;
template<class T> pthread_key_t ThreadSingleton<T>::key_;

template<class T> ThreadSingleton<T>::ThreadSingleton()
{
}

template<class T> ThreadSingleton<T>::~ThreadSingleton()
{
}

template<class T> 
T& ThreadSingleton<T>::instance()
{
	pthread_once(&once_,init);

	T* value = (T*)pthread_getspecific(key_);
	if(!value)
	{
		value = new T();
		THRCALL(::pthread_setspecific(key_,value));
	}
	return *value;
}

template<class T> void ThreadSingleton<T>::cleanUp(void* data)
{
	delete (T*)data;
	pthread_setspecific(key_,0);
}

template<class T> void ThreadSingleton<T>::init()
{
	pthread_key_create(&key_,cleanUp);
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

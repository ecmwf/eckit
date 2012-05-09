/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/AutoLock.h"
#include "eclib/ClassExtent.h"
#include "eclib/Exceptions.h"
#include "eclib/Mutex.h"

// We assume that global-initialisation is single threaded

template<class T>
typename ClassExtent<T>::Extent ClassExtent<T>::extent_;

template<class T>
ClassExtent<T>::ClassExtent(T* obj)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);
	extent_.map_[this] = obj;
}

template<class T>
ClassExtent<T>::~ClassExtent()
{

	if(extent_.inited_)  // This can be after exit() is called
						 // I need to find a solution
	{

	ASSERT(extent_.inited_);

	AutoLock<Mutex> lock(extent_.mutex_);
	ASSERT(extent_.map_.find(this) != extent_.map_.end());
	extent_.map_.erase(this);
	}
}

template<class T>
size_t ClassExtent<T>::size()
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);
	return extent_.map_.size();
}

template<class T>         
void ClassExtent<T>::callAll(void (T::*proc)())
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = map.begin(); i != map.end() ; ++i) 
        ((*i).second->*proc)();

}

template<class T>         
void ClassExtent<T>::callAll(void (T::*proc)() const)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = extent_.map_.begin();
		i != extent_.map_.end() ; ++i)
		((*i).second->*proc)();
}

template<class T> template<class P> 
void ClassExtent<T>::callAll(void (T::*proc)(P),P arg)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = map.begin(); i != map.end() ; ++i)
		((*i).second->*proc)(arg);
}

template<class T> template<class P> 
void ClassExtent<T>::callAll(void (T::*proc)(P) const,P arg)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = extent_.map_.begin();
		i != extent_.map_.end() ; ++i)
		((*i).second->*proc)(arg);
}

template<class T> template<class P1,class P2> 
void ClassExtent<T>::callAll(void (T::*proc)(P1,P2),P1 arg1,P2  arg2)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = map.begin(); i != map.end() ; ++i)
		((*i).second->*proc)(arg1,arg2);
}

template<class T> template<class P> 
void ClassExtent<T>::callAll(void (T::*proc)(P&),P& arg)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = extent_.map_.begin();
		i != extent_.map_.end() ; ++i)
		((*i).second->*proc)(arg);
}

template<class T> template<class P> 
void ClassExtent<T>::callAll(void (T::*proc)(P&) const,P& arg)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = extent_.map_.begin();
		i != extent_.map_.end() ; ++i)
		((*i).second->*proc)(arg);
}

template<class T> template<class P1,class P2> 
void ClassExtent<T>::callAll(void (T::*proc)(P1&,P2&),P1& arg1,P2&  arg2)
{
	ASSERT(extent_.inited_);
	AutoLock<Mutex> lock(extent_.mutex_);

    // Make a copy to cater for object that are deleted during the loop
    typename ClassExtent<T>::Extent::Map map = extent_.map_;
	// for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
	typedef  typename ClassExtent<T>::Extent::Map map_type;
	typename map_type::iterator i;
	for(i = map.begin(); i != map.end() ; ++i)
		((*i).second->*proc)(arg1,arg2);
}

template<class T>
ClassExtent<T>::Extent::Extent():
	inited_(true)
{
}

template<class T>
ClassExtent<T>::Extent::~Extent()
{
	inited_ = false;
}

// File changed by Amitav.
// Made the following change at several places:
//
// Replaced
//    for(ClassExtent<T>::Extent::Map::iterator i = extent_.map_.begin();
// with
//    typedef  typename ClassExtent<T>::Extent::Map map_type;
//    typename map_type::iterator i;
//    for(i = extent_.map_.begin();

/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClassExtent.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef ClassExtent_H
#define ClassExtent_H

#include "eclib/Mutex.h"
#include "eclib/NonCopyable.h"

template<class T>
class ClassExtent : private NonCopyable {
public:

// -- Contructors

	ClassExtent(T*);

// -- Destructor

	~ClassExtent();

// -- Methods
	
	static size_t size();

public: // methods
    
	static void callAll(void (T::*)());
	static void callAll(void (T::*)() const);

	template<class P> 
	static void callAll(void (T::*)(P),P);

	template<class P> 
	static void callAll(void (T::*)(P) const,P);

	template<class P1,class P2> 
	static void callAll(void (T::*)(P1,P2),P1,P2);

	template<class P> 
	static void callAll(void (T::*)(P&) const,P&);

	template<class P> 
	static void callAll(void (T::*)(P&),P&);

	template<class P1,class P2> 
	static void callAll(void (T::*)(P1&,P2&),P1&,P2&);

private: // members
    
	struct Extent {
		typedef map<ClassExtent<T>*,T*,less<ClassExtent<T>*> > Map;
		Mutex   mutex_;
		Map     map_;
		bool    inited_;
		Extent();
		~Extent();
	};
    
	static Extent extent_;

};

#include "ClassExtent.cc"

#endif

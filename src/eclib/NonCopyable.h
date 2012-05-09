/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_NonCopyable_H
#define eclib_NonCopyable_H

/// Inherit from this class to have a noncopyable class

template < typename T > 
class NonCopyable {
public:

    NonCopyable() {}
    
private:
    
    // No copy allowed    
    
    NonCopyable(const NonCopyable<T>&);
	NonCopyable<T>& operator=(const NonCopyable<T>&);

};

#endif // eclib_NonCopyable_h

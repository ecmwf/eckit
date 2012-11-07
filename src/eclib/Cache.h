/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Cache.h
// Tiago Quintino - (c) ECMWF Oct 12

#ifndef EcLib_Cache_H
#define EcLib_Cache_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"

//-----------------------------------------------------------------------------

template< typename K, typename V >
class Cache : private NonCopyable {

public: // types

    typedef K key_type;
    typedef V value_type;

public: // methods

    Cache();

    ~Cache();

    /// inserts an object in the cache
    /// @returns true if object was correctly inserted,
    ///          or false if key already existed and cannot be inserted
    bool insert( const K&, const V& );

    /// updates an object in the cache (or inserts if does not exist)
    /// @returns true if object existed and was updated,
    ///          false if the object did not exist and was inserted
    bool update( const K&, const V& );

    /// accesses an object in the cache
    /// @param v returns the object
    bool fetch( const K& key, V& v);

    /// removes an object from the cache
    /// @returns true if object was present and is removed
    bool evict( const K& );

    /// evicts entries that are considered expired
    void purge();

    /// evicts all entries
    void clear();

    /// @returns true if entry exists in cache
    bool exists( const K& ) const;

    /// @returns the number of entries in the cache
    size_t size() const;

protected: // methods

    void print(ostream&) const;

    friend ostream& operator<<(ostream& s,const Cache& p)
    {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

template< typename K, typename V >
Cache::Cache()
{
}

template< typename K, typename V >
Cache::~Cache()
{
}

template< typename K, typename V >
bool Cache::insert(const K &, const V &)
{
}

template< typename K, typename V >
bool Cache::fetch(const K &, V &)
{
}

template< typename K, typename V >
bool Cache::evict(const K &)
{
}

template< typename K, typename V >
void Cache::purge()
{
}

template< typename K, typename V >
void Cache::clear()
{
}

template< typename K, typename V >
bool Cache::exists(const K &) const
{
}

template< typename K, typename V >
size_t Cache::size() const
{
}

template< typename K, typename V >
void Cache::print(ostream &) const
{
}

//-----------------------------------------------------------------------------

#endif

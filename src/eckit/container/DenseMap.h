/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_DenseMap_h
#define eckit_DenseMap_h

/// @author Tiago Quintino

#include <vector>
#include <algorithm>
#include <utility>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template < typename K, typename V >
class DenseMap {

public: // types

	typedef K key_t;
	typedef V value_t;
	typedef std::pair<K,V> entry_t;
	typedef std::vector< entry_t > store_t;
	typedef typename store_t::iterator iterator;
	typedef typename store_t::const_iterator const_iterator;

public: // methods

  DenseMap( size_t s = 0 ) : sorted_(false)
  {
	  if(s > 0)
	    store_.reserve(s);
  }

  ~DenseMap() {}

  void reserve( size_t s ) { store_.reserve(s); }

  void insert( const K& k, const V& v )
  {
  	sorted_ = false;
    store_.push_back( std::make_pair(k,v) );
  }

  void replace( const K& k, const V& v )
  {
  	iterator it = find(k);
  	if( it != end() )
  	{
  		it->second = v;
  	}
  	else
  	{
	    store_.push_back( std::make_pair(k,v) );
  		sorted_ = false;
  	}
  }

  void clear() { store_.clear(); }

  size_t size() const { return store_.size(); }
  bool empty() const { return store_.size() == 0; }

  iterator begin() { return store_.begin(); }
  const_iterator cbegin() const { return store_.cbegin(); }

  iterator end() { return store_.end(); }
  const_iterator cend() const { return store_.cend(); }

  bool sorted() const { return sorted_; }

  void sort()
  {
  	if(!sorted_)
  	{
	  	std::sort( store_.begin(), store_.end(), LessThan() );
  		sorted_ = true;
  	}
  }

  V get( const K& k ) const { return find(k)->second; }

  iterator find( const K& k )
  {
  	if( !empty() )
  	{
	  	ASSERT(sorted_);
    	iterator it = std::lower_bound(store_.begin(), store_.end(), k, Compare());
    	if( it->first == k )
    		return it;
  	}    
    return end();
  }

  const_iterator find( const K& k ) const
  {
  	if( !empty() )
  	{
	  	ASSERT(sorted_);
    	const_iterator it = std::lower_bound(store_.cbegin(), store_.cend(), k, Compare());
    	if( it->first == k )
    		return it;
  	}    
    return cend();
  }

    friend std::ostream& operator<<(std::ostream& s, const DenseMap& m) { m.print(s);  return s; }

private: // methods

    void print(std::ostream& s) const
    {
    	const_iterator it = cbegin();
    	for( ; it != cend(); ++it )
    		s << it->first << " " << it->second << std::endl;
    }

private: // types

  class LessThan {
  public:
    bool operator() (const entry_t& e1, const entry_t& e2) const
    {
      return (e1.first < e2.first) ? true : false;
    }
  };

  class Compare {
  public:
    bool operator() (const entry_t& e, const K& k) const
    {
      return (e.first < k) ? true : false;
    }
    bool operator() (const K& k, const entry_t& e) const
    {
      return (e.first > k) ? true : false;
    }
  };

private: // members

  bool sorted_; 

  store_t store_; ///< storage of the inserted data

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

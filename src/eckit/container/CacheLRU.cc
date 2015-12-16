/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template< typename K, typename V>
CacheLRU<K,V>::CacheLRU(const size_t& maxSize) :
    maxSize_( maxSize )
{
}

template< typename K, typename V>
CacheLRU<K,V>::~CacheLRU()
{
    clear();
}

template< typename K, typename V>
bool CacheLRU<K,V>::insert(const key_type& key, const value_type& value)
{
    bool existed = false;

    typename map_type::iterator itr = map_.find(key);
    if( itr != map_.end() )
    {
        existed = true;

        // remove the key from where it is
        // we'll reinsert it again so it comes on top

        erase(itr);
    }

    storage_.push_front( Entry(key,value) );
    map_[key] = storage_.begin();

    trim();

    return existed;
}

template< typename K, typename V>
const V& CacheLRU<K,V>::access(const key_type& key)
{
    // check first the front() since it is the most popular/recent entry

    if( size() && storage_.front().key_ == key )
        return storage_.front().value_;

    typename map_type::iterator itr = map_.find(key);

    if(itr != map_.end())
    {
        // move entry of list to front
        // this keeps the most popular in front

        moveToFront(itr);

        return itr->second->value_;
    }
    else
    {
        throw eckit::OutOfRange( "key not present in CacheLRU", Here() );
    }
}

template< typename K, typename V>
bool CacheLRU<K,V>::remove(const key_type& key)
{
    bool existed = false;

    typename map_type::iterator itr = map_.find(key);
    if( itr != map_.end() )
    {
        existed = true;
        erase(itr);
    }

    return existed;
}

template< typename K, typename V>
bool CacheLRU<K,V>::exists(const key_type& key)
{
    return ( map_.find(key) != map_.end() );
}

template< typename K, typename V>
void CacheLRU<K,V>::clear()
{
    storage_.clear();
    map_.clear();
}

template< typename K, typename V>
void CacheLRU<K,V>::erase(typename map_type::iterator itr)
{
        storage_.erase( itr->second );
        map_.erase( itr );
}

template< typename K, typename V>
void CacheLRU<K,V>::trim()
{
    while(map_.size() > maxSize_) {
        map_.erase( storage_.back().key_ );
        storage_.pop_back();
    }
}

template< typename K, typename V>
void CacheLRU<K,V>::moveToFront(typename map_type::iterator itr)
{
    storage_.splice( storage_.begin(), storage_, itr->second );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Baudouin Raoult
/// @date   May 2015

#ifndef eckit_container_CacheManager_h
#define eckit_container_CacheManager_h

#include <string>
#include <functional>

#include "eckit/eckit.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Filesystem Cache Manager

template <class Traits>
class CacheManager : private NonCopyable {

public: // methods

    typedef typename Traits::value_type value_type;

    class CacheContentCreator {
    public:
        virtual void create(const PathName&, value_type& value) = 0;
    };

    typedef std::string key_t;

public: // methods

    explicit CacheManager(const PathName& root, bool throwOnCacheMiss);

    PathName getOrCreate(const key_t& key,
        CacheContentCreator& creator,
        value_type& value) const;

private: // methods


     bool get(const key_t& key, PathName& path) const;

     PathName stage(const key_t& key) const;

     bool commit(const key_t& key, const PathName& path) const;

     PathName entry(const key_t& key) const;


private: // members

    PathName root_;
    bool throwOnCacheMiss_;
};



//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/container/CacheManager.cc"
#endif

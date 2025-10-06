/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_RendezvousHash_H
#define eckit_utils_RendezvousHash_H

#include <map>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

/// This class implements the Rendezvous or Highest Random Weight (HRW) hashing
/// It is thread-safe, in terms that threads can add and remove nodes whilts others can
/// compute the hash and obtain the rendezvous node
///
/// @todo Make node a template parameter? Must be a serializable object

class RendezvousHash : private eckit::NonCopyable {

public:  // types

    typedef std::string Node;

    typedef std::map<std::string, std::string> Key;

    typedef std::string (*hash_func_ptr)(const std::string&);

private:  // types

    typedef std::vector<Node>::iterator iterator;

public:  // methods

    static std::string md5(const std::string& str);

    RendezvousHash(const hash_func_ptr hash = &md5);

    RendezvousHash(const std::vector<Node>& nodes, const hash_func_ptr hash = &md5);

    ~RendezvousHash();

    /// Provide a list of nodes / indices in the list of nodes for the given key

    void hashOrder(const Key& key, std::vector<Node>& nodes);

    void hashOrder(const Key& key, std::vector<size_t>& indices);

    /// Adds node to node list. No effect if node already present
    /// @returns true is node insertion was successful
    bool addNode(const Node& node);

    /// Removes node from node list. No effect if node not present
    /// @returns true is node removal was successful
    bool removeNode(const Node& node);

private:  // methods

    std::string flatten(const Key&) const;

    void hashOrderInternal(const Key& key, std::vector<size_t>& indexes);

private:  // types

    eckit::Mutex mutex_;  //< protects addition and removal of nodes

    hash_func_ptr hash_;

    std::vector<Node> nodes_;
};

//----------------------------------------------------------------------------------------------------------------------


}  // end namespace eckit

#endif

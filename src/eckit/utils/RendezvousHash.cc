/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <numeric>

#include "eckit/utils/RendezvousHash.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/types/Types.h"
#include "eckit/utils/MD5.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

std::string RendezvousHash::md5(const std::string& str) {
    eckit::MD5 md5(str.c_str(), str.size());
    return md5.digest();
}

RendezvousHash::RendezvousHash(const RendezvousHash::hash_func_ptr hash) :
    hash_(hash) {}

RendezvousHash::RendezvousHash(const std::vector<RendezvousHash::Node>& nodes,
                               const RendezvousHash::hash_func_ptr hash) :
    hash_(hash), nodes_(nodes) {}

RendezvousHash::~RendezvousHash() {}

void RendezvousHash::hashOrder(const RendezvousHash::Key& key, std::vector<RendezvousHash::Node>& nodes) {

    std::vector<size_t> indexes;

    AutoLock<Mutex> lock(mutex_);
    hashOrder(key, indexes);

    ASSERT(indexes.size() == nodes_.size());

    nodes.clear();
    nodes.reserve(indexes.size());

    for (size_t idx : indexes) {
        nodes.push_back(nodes_[idx]);
    }
}

void RendezvousHash::hashOrder(const RendezvousHash::Key& key, std::vector<size_t>& indices) {

    AutoLock<Mutex> lock(mutex_);
    hashOrderInternal(key, indices);
}

void RendezvousHash::hashOrderInternal(const RendezvousHash::Key& key, std::vector<size_t>& indices) {

    if (nodes_.size() == 0) {
        throw BadParameter("Cannot return hashed order with no nodes", Here());
    }

    // n.b. Does not do locking. That is delegated to the public calling functions.

    indices.resize(nodes_.size());
    std::iota(indices.begin(), indices.end(), 0);

    // We hash based on a stringized key
    std::string skey = flatten(key);

    std::vector<std::string> hashes;
    hashes.reserve(nodes_.size());

    // Calculate hashes of the key with each of the available nodes

    for (const auto& node : nodes_) {
        std::string toHash = skey + "+" + node;
        hashes.emplace_back(hash_(toHash));
        Log::debug<LibEcKit>() << "node=" << node << ", str=" << toHash << ", hash=" << hashes.back() << std::endl;
    }

    // TODO: We don't necessarily need N results, could do a subset.
    std::sort(indices.begin(), indices.end(), [&hashes](size_t lhs, size_t rhs) { return hashes[lhs] < hashes[rhs]; });
}

bool RendezvousHash::addNode(const RendezvousHash::Node& node) {
    AutoLock<Mutex> lock(mutex_);

    auto it = std::find(nodes_.begin(), nodes_.end(), node);
    if (it == nodes_.end()) {
        nodes_.push_back(node);
        return true;
    }

    return false;
}

bool RendezvousHash::removeNode(const RendezvousHash::Node& node) {
    AutoLock<Mutex> lock(mutex_);

    auto it = std::find(nodes_.begin(), nodes_.end(), node);
    if (it != nodes_.end()) {
        nodes_.erase(it);
        return true;
    }

    return false;
}


std::string RendezvousHash::flatten(const RendezvousHash::Key& key) const {
    std::ostringstream flat;

    for (Key::const_iterator itr = key.begin(); itr != key.end(); ++itr) {
        flat << "/" << itr->first << ":" << itr->second;
    }

    return flat.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

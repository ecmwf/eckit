/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <map>

#include "eckit/utils/Hash.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

    HashFactory::HashFactory() {
    }

    HashFactory& HashFactory::instance() {
        static HashFactory theOne;
        return theOne;
    }

    void HashFactory::add(const std::string& name, HashBuilderBase* builder) {
        AutoLock<Mutex> lock(mutex_);
        if(has(name)) {
            throw SeriousBug("Duplicate entry in HashFactory: " + name, Here());
        }
        builders_[name] = builder;
    }

    void HashFactory::remove(const std::string& name) {
        builders_.erase(name);
    }

    bool HashFactory::has(const std::string& name) {
        AutoLock<Mutex> lock(mutex_);
        return builders_.find(name) != builders_.end();
    }

    void HashFactory::list(std::ostream& out) {
        AutoLock<Mutex> lock(mutex_);
        const char* sep = "";
        for (std::map<std::string, HashBuilderBase*>::const_iterator j = builders_.begin(); j != builders_.end(); ++j) {
            out << sep << (*j).first;
            sep = ", ";
        }
    }

    Hash* HashFactory::build() {

        std::string compression = eckit::Resource<std::string>("defaultHash;ECKIT_DEFAULT_HASH", "MD5");

        if(has(compression)) {
            return build(compression);
        }

        return build("none");
    }

    Hash* HashFactory::build(const std::string& name) {

        AutoLock<Mutex> lock(mutex_);

        auto j = builders_.find(name);

        eckit::Log::debug() << "Looking for HashBuilder [" << name << "]" << std::endl;

        if (j == builders_.end()) {
            eckit::Log::error() << "No HashBuilder for [" << name << "]" << std::endl;
            eckit::Log::error() << "HashBuilders are:" << std::endl;
            for (j = builders_.begin(); j != builders_.end(); ++j)
                eckit::Log::error() << "   " << (*j).first << std::endl;
            throw eckit::SeriousBug(std::string("No HashBuilder called ") + name);
        }

        return (*j).second->make();
    }

    Hash* HashFactory::build(const std::string& name, const std::string& param) {

        AutoLock<Mutex> lock(mutex_);

        auto j = builders_.find(name);

        eckit::Log::debug() << "Looking for HashBuilder [" << name << "]" << std::endl;

        if (j == builders_.end()) {
            eckit::Log::error() << "No HashBuilder for [" << name << "]" << std::endl;
            eckit::Log::error() << "HashBuilders are:" << std::endl;
            for (j = builders_.begin(); j != builders_.end(); ++j)
                eckit::Log::error() << "   " << (*j).first << std::endl;
            throw eckit::SeriousBug(std::string("No HashBuilder called ") + name);
        }

        return (*j).second->make(param);
    }

//----------------------------------------------------------------------------------------------------------------------

    HashBuilderBase::HashBuilderBase(const std::string& name) : name_(name) {
        HashFactory::instance().add(name_, this);
    }

    HashBuilderBase::~HashBuilderBase() {
        HashFactory::instance().remove(name_);
    }

//----------------------------------------------------------------------------------------------------------------------

Hash::Hash() {}

Hash::~Hash() {}

//----------------------------------------------------------------------------------------------------------------------

NoHash::NoHash() {}

NoHash::NoHash(const std::string&) {}

NoHash::~NoHash() {}

void NoHash::reset() const {}

Hash::digest_t NoHash::compute(const void*, long) {
    return std::string();
}

void NoHash::update(const void*, long) {}

Hash::digest_t NoHash::digest() const {
    return digest_;  // should be empty
}

namespace {
HashBuilder<NoHash> builder1("None");
HashBuilder<NoHash> builder2("NoHash");
}  // namespace
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

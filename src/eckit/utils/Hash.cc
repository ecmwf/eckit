/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/utils/Hash.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex *local_mutex = 0;
static std::map<std::string, HashFactory *> *m = 0;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, HashFactory *>();
}


HashFactory::HashFactory(const std::string& name):
    name_(name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());

    (*m)[name] = this;
}


HashFactory::~HashFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}

bool HashFactory::has(const std::string& name)
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    return m->find(name) != m->end();
}

void HashFactory::list(std::ostream& out) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (std::map<std::string, HashFactory *>::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}


Hash *HashFactory::build(const std::string &name) {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    std::map<std::string, HashFactory *>::const_iterator j = m->find(name);

    eckit::Log::debug() << "Looking for HashFactory [" << name << "]" << std::endl;

    if (j == m->end()) {
        eckit::Log::error() << "No HashFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "HashFactories are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No HashFactory called ") + name);
    }

    return (*j).second->make();
}

//----------------------------------------------------------------------------------------------------------------------

Hash::Hash() {
}

Hash::~Hash() {
}

//----------------------------------------------------------------------------------------------------------------------

NoHash::NoHash() {
}

NoHash::~NoHash() {
}

Hash::digest_t NoHash::compute(const void*, long) {
    return std::string();
}

void NoHash::add(const void*, long) {
}

Hash::digest_t NoHash::digest() const {
    return digest_; // should be empty
}

namespace  {
    HashBuilder<NoHash> builder1("None");
    HashBuilder<NoHash> builder2("NoHash");
}
//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

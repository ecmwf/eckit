/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/DataBlob.h"
#include "eckit/io/DataHandle.h"
#include "eckit/memory/MemoryPool.h"
#include "eckit/thread/AutoLock.h"

#include <string>
#include <map>

namespace eckit {

// -------------------------------------------------------------------------------------------------

namespace {
    Mutex* local_mutex = 0;
    std::map<std::string, DataBlobFactory*> *m = 0;
    pthread_once_t once = PTHREAD_ONCE_INIT;
    void init() {
        local_mutex = new Mutex();
        m = new std::map<std::string, DataBlobFactory*>();
    }
}

/*
 * When a concrete instance of a DataBlobFactory is instantiated (in practice
 * a DataBlobBuilder<>) add it to the list of available factories.
 */
DataBlobFactory::DataBlobFactory(const std::string& name) :
    name_(name) {

    pthread_once(&once, init);

    AutoLock<Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}

DataBlobFactory::~DataBlobFactory() {
    AutoLock<Mutex> lock(local_mutex);
    m->erase(name_);
}

void DataBlobFactory::list(std::ostream& out) {

    pthread_once(&once, init);

    AutoLock<Mutex> lock(local_mutex);

    const char* sep = "";
    for (std::map<std::string, DataBlobFactory*>::const_iterator j = m->begin(); j != m->end(); ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}


const DataBlobFactory& DataBlobFactory::findFactory(const std::string& name) {

    pthread_once(&once, init);

    AutoLock<Mutex> lock(local_mutex);

    Log::info() << "Looking for DataBlobFactory [" << name << "]" << std::endl;

    std::map<std::string, DataBlobFactory *>::const_iterator j = m->find(name);
    if (j == m->end()) {
        Log::error() << "No DataBlobFactory for [" << name << "]" << std::endl;
        Log::error() << "DataBlobFactories are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
            Log::error() << "   " << (*j).first << std::endl;
        throw SeriousBug(std::string("No DataBlobFactory called ") + name);
    }

    return *(*j).second;
}


DataBlob* DataBlobFactory::build(const std::string &name, const void* data, size_t length) {

    const DataBlobFactory& factory(findFactory(name));

    return factory.make(data, length);
}


DataBlob* DataBlobFactory::build(const std::string &name, DataHandle& dh, size_t length) {

    const DataBlobFactory& factory(findFactory(name));

    return factory.make(dh, length);
}

// -------------------------------------------------------------------------------------------------

DataBlob::DataBlob(size_t length) :
    buffer_(length) {
}

DataBlob::DataBlob(const void* data, size_t length) :
    buffer_((const char*)data, length) {
}

DataBlob::DataBlob(DataHandle& dh, size_t length) :
    buffer_(length) {

    dh.read(buffer_, length);
}

DataBlob::~DataBlob() {
}

const Buffer& DataBlob::buffer() const {
    return buffer_;
}

Buffer& DataBlob::buffer() {
    return buffer_;
}

size_t DataBlob::length() const { return buffer_.size(); }

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit


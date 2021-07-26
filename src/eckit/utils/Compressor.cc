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

#include "eckit/utils/Compressor.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/utils/StringTools.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CompressorFactory::CompressorFactory() {}

CompressorFactory& CompressorFactory::instance() {
    static CompressorFactory theOne;
    return theOne;
}

void CompressorFactory::add(const std::string& name, CompressorBuilderBase* builder) {
    std::string nameLowercase = StringTools::lower(name);

    AutoLock<Mutex> lock(mutex_);
    if (has(nameLowercase)) {
        throw SeriousBug("Duplicate entry in CompressorFactory: " + nameLowercase, Here());
    }
    builders_[nameLowercase] = builder;
}

void CompressorFactory::remove(const std::string& name) {
    std::string nameLowercase = StringTools::lower(name);

    AutoLock<Mutex> lock(mutex_);
    builders_.erase(nameLowercase);
}

bool CompressorFactory::has(const std::string& name) {
    std::string nameLowercase = StringTools::lower(name);

    AutoLock<Mutex> lock(mutex_);
    return builders_.find(nameLowercase) != builders_.end();
}

void CompressorFactory::list(std::ostream& out) {
    AutoLock<Mutex> lock(mutex_);
    const char* sep = "";
    for (std::map<std::string, CompressorBuilderBase*>::const_iterator j = builders_.begin(); j != builders_.end();
         ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

Compressor* CompressorFactory::build() {

    std::string compression = eckit::Resource<std::string>("defaultCompression;ECKIT_DEFAULT_COMPRESSION", "snappy");

    if (has(compression)) {
        return build(compression);
    }

    return build("none");
}

Compressor* CompressorFactory::build(const std::string& name) {
    std::string nameLowercase = StringTools::lower(name);

    AutoLock<Mutex> lock(mutex_);

    auto j = builders_.find(nameLowercase);

    eckit::Log::debug() << "Looking for CompressorBuilder [" << nameLowercase << "]" << std::endl;

    if (j == builders_.end()) {
        eckit::Log::error() << "No CompressorBuilder for [" << nameLowercase << "]" << std::endl;
        eckit::Log::error() << "CompressorBuilders are:" << std::endl;
        for (j = builders_.begin(); j != builders_.end(); ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No CompressorBuilder called ") + nameLowercase);
    }

    return (*j).second->make();
}


//----------------------------------------------------------------------------------------------------------------------

CompressorBuilderBase::CompressorBuilderBase(const std::string& name) :
    name_(name) {
    CompressorFactory::instance().add(name_, this);
}

CompressorBuilderBase::~CompressorBuilderBase() {
    CompressorFactory::instance().remove(name_);
}

//----------------------------------------------------------------------------------------------------------------------

Compressor::Compressor() {}

Compressor::~Compressor() {}

//----------------------------------------------------------------------------------------------------------------------

NoCompressor::NoCompressor() {}

size_t NoCompressor::compress(const void* in, size_t len, Buffer& out) const {
    if (out.size() < len) {
        out.resize(len);
    }
    out.copy(in, len);
    return len;
}

void NoCompressor::uncompress(const void* in, size_t len, Buffer& out, size_t outlen) const {
    ASSERT(outlen == len);
    if (out.size() < outlen) {
        out.resize(outlen);
    }
    out.copy(in, len);
}

//----------------------------------------------------------------------------------------------------------------------

namespace {

CompressorBuilder<NoCompressor> builder1("none");

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

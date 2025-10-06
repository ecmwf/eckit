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

#include "eckit/utils/Compressor.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/utils/StringTools.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CompressorFactory::CompressorFactory() = default;

CompressorFactory& CompressorFactory::instance() {
    static CompressorFactory theOne;
    return theOne;
}

void CompressorFactory::add(const std::string& name, CompressorBuilderBase* builder) {
    AutoLock<Mutex> lock(mutex_);

    auto key = StringTools::lower(name);
    if (has(key)) {
        throw SeriousBug("Duplicate entry in CompressorFactory: " + key, Here());
    }
    builders_[key] = builder;
}

void CompressorFactory::remove(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);

    auto key = StringTools::lower(name);
    builders_.erase(key);
}

bool CompressorFactory::has(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);

    auto key = StringTools::lower(name);
    return builders_.find(key) != builders_.end();
}

std::vector<std::string> CompressorFactory::keys() const {
    AutoLock<Mutex> lock(mutex_);

    std::vector<std::string> keys;
    for (const auto& builder : builders_) {
        keys.push_back(builder.first);
    }
    return keys;
}

void CompressorFactory::list(std::ostream& out) {
    AutoLock<Mutex> lock(mutex_);

    const auto* sep = "";
    for (const auto& j : builders_) {
        out << sep << j.first;
        sep = ", ";
    }
}

Compressor* CompressorFactory::build() {
    std::string compression = Resource<std::string>("defaultCompression;ECKIT_DEFAULT_COMPRESSION", "snappy");

    return build(has(compression) ? compression : "none");
}

Compressor* CompressorFactory::build(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);

    auto key = StringTools::lower(name);
    if (auto j = builders_.find(key); j != builders_.end()) {
        return (*j).second->make();
    }

    list(Log::error() << "No CompressorBuilder for [" << key << "]. CompressorBuilders are:");
    Log::error() << std::endl;

    throw SeriousBug(std::string("No CompressorBuilder called ") + key);
}


//----------------------------------------------------------------------------------------------------------------------

CompressorBuilderBase::CompressorBuilderBase(const std::string& name) : name_(name) {
    CompressorFactory::instance().add(name_, this);
}

CompressorBuilderBase::~CompressorBuilderBase() {
    CompressorFactory::instance().remove(name_);
}

//----------------------------------------------------------------------------------------------------------------------

NoCompressor::NoCompressor() = default;

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

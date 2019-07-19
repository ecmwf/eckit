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

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CompressorFactory::CompressorFactory() {
}

CompressorFactory& CompressorFactory::instance() {
    static CompressorFactory theOne;
    return theOne;
}

void CompressorFactory::add(const std::string& name, CompressorBuilderBase* builder) {
    AutoLock<Mutex> lock(mutex_);
    if(has(name)) {
        throw SeriousBug("Duplicate entry in CompressorFactory: " + name, Here());
    }
    builders_[name] = builder;
}

void CompressorFactory::remove(const std::string& name) {
    builders_.erase(name);
}

bool CompressorFactory::has(const std::string& name) {
    AutoLock<Mutex> lock(mutex_);
    return builders_.find(name) != builders_.end();
}

void CompressorFactory::list(std::ostream& out) {
    AutoLock<Mutex> lock(mutex_);
    const char* sep = "";
    for (std::map<std::string, CompressorBuilderBase*>::const_iterator j = builders_.begin(); j != builders_.end(); ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

Compressor* CompressorFactory::build(const std::string& name) {

    AutoLock<Mutex> lock(mutex_);

    auto j = builders_.find(name);

    eckit::Log::debug() << "Looking for CompressorBuilder [" << name << "]" << std::endl;

    if (j == builders_.end()) {
        eckit::Log::error() << "No CompressorBuilder for [" << name << "]" << std::endl;
        eckit::Log::error() << "CompressorBuilders are:" << std::endl;
        for (j = builders_.begin(); j != builders_.end(); ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No CompressorBuilder called ") + name);
    }

    return (*j).second->make();
}


//----------------------------------------------------------------------------------------------------------------------

CompressorBuilderBase::CompressorBuilderBase(const std::string& name) : name_(name) {
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

void NoCompressor::compress(const Buffer& in, Buffer& out) const
{
    ASSERT(out.size() >= in.size());
    ::memcpy(out, in, in.size());
}

void NoCompressor::uncompress(const Buffer& in, Buffer& out) const
{
    ASSERT(out.size() >= in.size());
    ::memcpy(out, in, in.size());
}

//----------------------------------------------------------------------------------------------------------------------

namespace {

CompressorBuilder<NoCompressor> builder1("None");
CompressorBuilder<NoCompressor> builder2("NoCompressor");

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

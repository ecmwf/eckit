/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MarsFSFile.cc
// Baudouin Raoult - (c) ECMWF Jun 11

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/marsfs/MarsFSFile.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MarsFSFile::MarsFSFile(const MarsFSPath& path) : MarsFSClient(path), path_(path), lock_(connector_) {
    static std::string marsFsHashing = eckit::Resource<std::string>("marsFsHashing", "None");
    hash_.reset(eckit::HashFactory::instance().build(marsFsHashing));

    connector_.autoclose(true); // Will close the connector
}

MarsFSFile::~MarsFSFile() {}


void MarsFSFile::print(std::ostream& s) const {
    s << path_;
}

Length MarsFSFile::open(const char* mode, bool overwrite) {
    //    Log::info() << "MarsFSFile::open " << path_ << " " << mode << std::endl;
    Stream& s = connector_;
    int port;
    unsigned long long length;

    s << "open";
    s << path_.path();
    s << mode;
    s << overwrite;
    s >> port;

    Log::info() << "MarsFSFile::open " << path_ << " " << mode << ", host=" << connector_.host() << ", port=" << port
                << std::endl;
    data_.connect(connector_.host(), port);

    Log::info() << "Connected with socket " << data_ << std::endl;

    s >> length;

    Log::info() << "File length is " << length << std::endl;

    return length;
}

long MarsFSFile::read(void* buffer, long len) {

    Stream& s = connector_;
    long size;

    // Log::info() << "MarsFSFile::read " << len << std::endl;
    s << "read";
    s << len;

    s >> size;

    ASSERT(data_.isConnected());
    ASSERT(data_.read(buffer, size) == size);

    // hash integrety check
    {
        // Timer t("MarsFSFile::read() hashing");
        std::string remoteHash;
        s >> remoteHash;

        const std::string localHash = hash_->compute(buffer, size);

        if (localHash != remoteHash) {
            std::ostringstream msg;
            msg << "Mismatch of hash while reading from MarsFS:"
                << " expected remote hash " << remoteHash << " local computed hash " << localHash;
            throw eckit::BadValue(msg.str(), Here());
        }
    }
    return size;
}

long MarsFSFile::write(const void* buffer, long len) {

    Stream& s = connector_;
    long size;

    s << "write";

    s << len;

    ASSERT(data_.isConnected());
    ASSERT(data_.write(buffer, len) == len);

    // hash integrety check
    {
        // Timer t("MarsFSFile::write() hashing");
        s << hash_->compute(buffer, len);
    }

    s >> size;

    return size;
}

Offset MarsFSFile::seek(const Offset& pos) {
    Stream& s = connector_;
    // Log::info() << "MarsFSFile::seek " << pos << std::endl;

    unsigned long long llpos = pos;

    s << "seek";
    s << llpos;

    s >> llpos;

    return llpos;
}

void MarsFSFile::skip(const Length& n) {
    Stream& s = connector_;
    // Log::info() << "MarsFSFile::skip " << n << std::endl;

    unsigned long long p = n;
    bool ok;

    s << "skip";
    s << p;
    s >> ok;
}

void MarsFSFile::close() {
    Stream& s = connector_;

    // Make sure to close the connection *before* the server

    data_.close();

    bool ok;
    s << "close";
    s >> ok;
}

Length MarsFSFile::length() {
    return size(path_.path());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

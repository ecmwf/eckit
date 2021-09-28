/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"

#include "eckit/io/StatsHandle.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StatsHandle::StatsHandle(DataHandle& handle) :
    HandleHolder(handle),
    reads_(0),
    seeks_(0),
    writes_(0),
    //    positions_(0),
    bytesRead_(0),
    bytesWritten_(0),
    timer_(),
    readTime_(0),
    writeTime_(0),
    seekTime_(0) {}

StatsHandle::StatsHandle(DataHandle* handle) :
    HandleHolder(handle),
    reads_(0),
    seeks_(0),
    writes_(0),
    //    positions_(0),
    bytesRead_(0),
    bytesWritten_(0),
    timer_(),
    readTime_(0),
    writeTime_(0),
    seekTime_(0) {}

StatsHandle::~StatsHandle() {
    std::cout << "StatsHandle for " << handle() << std::endl;
    std::cout << "       Elapsed: " << eckit::Seconds(timer_.elapsed()) << std::endl;

    if (reads_) {
        std::cout << "  No. of reads: " << eckit::BigNum(reads_) << std::endl;
        std::cout << "    Bytes read: " << eckit::Bytes(bytesRead_) << std::endl;
        std::cout << "  Average read: " << eckit::Bytes(bytesRead_ / reads_) << std::endl;
        std::cout << "     Read time: " << eckit::Seconds(readTime_) << std::endl;
        std::cout << "     Read rate: " << eckit::Bytes(bytesRead_, readTime_) << std::endl;
    }

    if (writes_) {
        std::cout << " No. of writes: " << eckit::BigNum(writes_) << std::endl;
        std::cout << " Bytes written: " << eckit::Bytes(bytesWritten_) << std::endl;
        std::cout << " Average write: " << eckit::Bytes(bytesWritten_ / writes_) << std::endl;
        std::cout << "    Write time: " << eckit::Seconds(writeTime_) << std::endl;
        std::cout << "    Write rate: " << eckit::Bytes(bytesWritten_, writeTime_) << std::endl;
    }

    if (seeks_) {
        std::cout << "  No. of seeks: " << eckit::BigNum(seeks_) << std::endl;
        std::cout << "     Seek time: " << eckit::Seconds(seekTime_) << std::endl;
    }
}

void StatsHandle::print(std::ostream& s) const {
    /*
    if(format(s) == Log::compactFormat)
        s << "StatsHandle";
    else*/
    s << "StatsHandle[handle=" << handle() << ']';
}

Length StatsHandle::openForRead() {
    return handle().openForRead();
}

void StatsHandle::openForWrite(const Length& l) {
    handle().openForWrite(l);
}

void StatsHandle::openForAppend(const Length& l) {
    handle().openForAppend(l);
}

long StatsHandle::read(void* data, long len) {
    double x = timer_.elapsed();
    reads_++;
    bytesRead_ += len;
    long ret = handle().read(data, len);
    readTime_ += timer_.elapsed() - x;
    return ret;
}

long StatsHandle::write(const void* data, long len) {
    double x = timer_.elapsed();
    writes_++;
    bytesWritten_ += len;
    long ret = handle().write(data, len);
    writeTime_ += timer_.elapsed() - x;
    return ret;
}

void StatsHandle::close() {
    handle().close();
}

void StatsHandle::flush() {
    return handle().flush();
}

Length StatsHandle::estimate() {
    return handle().estimate();
}

Offset StatsHandle::position() {
    return handle().position();
}

Offset StatsHandle::seek(const Offset& o) {
    double x = timer_.elapsed();
    seeks_++;
    Offset ret = handle().seek(o);
    seekTime_ += timer_.elapsed() - x;
    return ret;
}

void StatsHandle::skip(const Length& n) {
    double x = timer_.elapsed();
    seeks_++;
    handle().skip(n);
    seekTime_ += timer_.elapsed() - x;
}

void StatsHandle::rewind() {
    double x = timer_.elapsed();
    seeks_++;
    handle().rewind();
    seekTime_ += timer_.elapsed() - x;
}

void StatsHandle::restartReadFrom(const Offset& o) {
    return handle().restartReadFrom(o);
}

void StatsHandle::restartWriteFrom(const Offset& o) {
    return handle().restartWriteFrom(o);
}

DataHandle* StatsHandle::clone() const {
    NOTIMP;
}

std::string StatsHandle::name() const {
    return handle().name();
}


bool StatsHandle::compress(bool b) {
    return handle().compress(b);
}

bool StatsHandle::merge(DataHandle*) {
    NOTIMP;
}

bool StatsHandle::isEmpty() const {
    return handle().isEmpty();
}


bool StatsHandle::moveable() const {
    return false;
}

const std::set<std::string>& StatsHandle::requiredMoverAttributes() const {
    // Requires moveable();
    NOTIMP;
}

void StatsHandle::toLocal(Stream& s) const {
    NOTIMP;
}

DataHandle* StatsHandle::toLocal() {
    NOTIMP;
}

void StatsHandle::toRemote(Stream& s) const {
    NOTIMP;
}

void StatsHandle::cost(std::map<std::string, Length>&, bool) const {
    NOTIMP;
}

std::string StatsHandle::title() const {
    return handle().title();
}

void StatsHandle::collectMetrics(const std::string& what) const {
    handle().collectMetrics(what);
}

Length StatsHandle::saveInto(DataHandle& other, TransferWatcher& watcher) {
    return handle().saveInto(other, watcher);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

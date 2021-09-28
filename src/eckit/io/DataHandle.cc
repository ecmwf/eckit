/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <cstring>

#include "eckit/eckit.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/DblBuffer.h"
#include "eckit/io/MoverTransfer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Progress.h"
#include "eckit/log/Timer.h"
#include "eckit/runtime/Metrics.h"


namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

AutoClose::~AutoClose() noexcept(false) {
    bool fail = !Exception::throwing();

    try {
        handle_.close();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        if (fail) {
            Log::error() << "** Exception is re-thrown" << std::endl;
            throw;
        }
        Log::error() << "** An exception is already in progress" << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------


ClassSpec DataHandle::classSpec_ = {
    &Streamable::classSpec(),
    "DataHandle",
};
Reanimator<DataHandle> DataHandle::reanimator_;

DataHandle::DataHandle() {}

DataHandle::DataHandle(Stream& s) :
    Streamable(s) {}

void DataHandle::encode(Stream& s) const {
    Streamable::encode(s);
}

void DataHandle::flush() {
    std::ostringstream os;
    os << "DataHandle::flush() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Length DataHandle::saveInto(DataHandle& other, TransferWatcher& watcher) {

    static const bool moverTransfer = Resource<bool>("-mover;moverTransfer", 0);

    compress();

    Log::status() << Bytes(estimate()) << " " << title() << " => " << other.title() << std::endl;

    if (moverTransfer && moveable() && other.moveable()) {
        Log::debug<LibEcKit>() << "Using MoverTransfer" << std::endl;
        MoverTransfer mover(watcher);
        return mover.transfer(*this, other);
    }


    static const bool doubleBuffer = Resource<bool>("doubleBuffer", 0);

    if (doubleBuffer && doubleBufferOK() && other.doubleBufferOK()) {
        static const long bufsize = Resource<long>("doubleBufferSize", 10 * 1024 * 1024 / 20);
        static const long count   = Resource<long>("doubleBufferCount", 20);

        Metrics::set("double_buffering", true);

        DblBuffer buf(count, bufsize, watcher);
        return buf.copy(*this, other);
    }
    else {

        static const long bufsize = Resource<long>("bufferSize;$ECKIT_DATAHANDLE_SAVEINTO_BUFFER_SIZE", 64 * 1024 * 1024);

        Buffer buffer(bufsize);

        watcher.watch(0, 0);

        Length estimate = openForRead();
        AutoClose closer1(*this);
        watcher.fromHandleOpened();
        other.openForWrite(estimate);
        AutoClose closer2(other);
        watcher.toHandleOpened();

        Progress progress("Moving data", 0, estimate);

        Length total     = 0;
        long length      = -1;
        double readTime  = 0;
        double lastRead  = 0;
        double writeTime = 0;
        double lastWrite = 0;
        Timer timer("Save into");
        bool more = true;

        while (more) {
            more = false;
            try {
                while ((length = read(buffer, buffer.size())) > 0) {
                    double r = timer.elapsed() - lastRead;
                    readTime += r;
                    lastWrite = timer.elapsed();

                    if (other.write((const char*)buffer, length) != length)
                        throw WriteError(name() + " into " + other.name());

                    double w = timer.elapsed() - lastWrite;
                    writeTime += w;
                    total += length;
                    progress(total);
                    watcher.watch(buffer, length);
                    lastRead = timer.elapsed();

                    Bytes rRate(total, readTime);
                    Bytes wRate(total, writeTime);

                    Log::message() << rRate.shorten() << " " << wRate.shorten() << std::endl;
                }
            }
            catch (RestartTransfer& retry) {
                Log::warning() << "Retrying transfer from " << retry.from() << " (" << Bytes(retry.from()) << ")"
                               << std::endl;

                restartReadFrom(retry.from());
                other.restartWriteFrom(retry.from());
                watcher.restartFrom(retry.from());

                Log::warning() << "Total so far " << total << std::endl;
                total = Length(0) + retry.from();
                Log::warning() << "New total " << total << std::endl;
                more = true;
            }
        }

        Log::message() << "" << std::endl;


        Log::info() << "Read  rate: " << Bytes(total, readTime) << std::endl;
        Log::info() << "Write rate: " << Bytes(total, writeTime) << std::endl;

        if (length < 0)
            throw ReadError(name() + " into " + other.name());

        if (estimate != 0 && estimate != total) {
            std::ostringstream os;
            os << "DataHandle::saveInto got " << total << " bytes out of " << estimate;
            throw ReadError(name() + " into " + other.name() + " " + os.str());
        }

        this->collectMetrics("source");
        other.collectMetrics("target");
        Metrics::set("size", total);
        Metrics::set("time", timer.elapsed());
        Metrics::set("read_time", readTime);
        Metrics::set("write_time", writeTime);
        Metrics::set("double_buffering", false);


        return total;
    }
}

Length DataHandle::saveInto(const PathName& path, TransferWatcher& w) {
    std::unique_ptr<DataHandle> file{path.fileHandle()};
    return saveInto(*file, w);
}

Length DataHandle::copyTo(DataHandle& other, long bufsize) {

    Buffer buffer(bufsize);

    Length estimate = openForRead();
    AutoClose closer1(*this);
    other.openForWrite(estimate);
    AutoClose closer2(other);

    Length total = 0;
    long length  = -1;

    while ((length = read(buffer, buffer.size())) > 0) {

        if (other.write((const char*)buffer, length) != length)
            throw WriteError(name() + " into " + other.name());

        total += length;
    }

    if (length < 0)
        throw ReadError(name() + " into " + other.name());

    if (estimate != 0 && estimate != total) {
        std::ostringstream os;
        os << "DataHandle::copyTo got " << total << " bytes out of " << estimate;
        throw ReadError(name() + " into " + other.name() + " " + os.str());
    }

    return total;
}

Length DataHandle::copyTo(DataHandle& other) {

    static const long bufsize = Resource<long>("bufferSize", 64 * 1024 * 1024);
    return copyTo(other, bufsize);
}


std::string DataHandle::name() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

std::string DataHandle::title() const {
    return className();
}

std::string DataHandle::metricsTag() const {
    return title();
}

void DataHandle::collectMetrics(const std::string& what) const {
    Metrics::set(what, this->metricsTag());
}

template <>
Streamable* Reanimator<DataHandle>::ressucitate(Stream& s) const {
    return nullptr;
}

bool DataHandle::compare(DataHandle& other) {
    size_t bufsize = static_cast<size_t>(Resource<long>("compareBufferSize", 10 * 1024 * 1024));

    Buffer buffer1(bufsize);
    Buffer buffer2(bufsize);

    DataHandle& self = *this;

    Length estimate1 = self.openForRead();
    AutoClose closer1(self);
    Length estimate2 = other.openForRead();
    AutoClose closer2(other);

    if (estimate1 != estimate2) {
        Log::error() << "DataHandle::compare(" << self << "," << other << ") failed: openForRead() returns "
                     << estimate1 << " and " << estimate2 << std::endl;
        return false;
    }

    Log::status() << "Comparing data" << std::endl;

    Progress progress("Comparing data", 0, estimate1);
    Length total = 0;

    for (;;) {
        long len1 = self.read(buffer1, buffer1.size());
        long len2 = other.read(buffer2, buffer2.size());

        if (len1 != len2) {
            Log::error() << "DataHandle::compare(" << self << "," << other << ") failed: read() returns " << len1
                         << " and " << len2 << std::endl;
            return false;
        }

        if (len1 <= 0 && len2 <= 0) {
            Log::info() << "DataHandle::compare(" << self << "," << other << ") is successful" << std::endl;
            return true;
        }

        total += len1;
        progress(total);

        if (::memcmp(buffer1, buffer2, len1)) {
            Log::error() << "DataHandle::compare(" << self << "," << other
                         << ") failed: memcmp() returns non-zero value" << std::endl;
            return false;
        }
    }

    // should never arrive here
}

Offset DataHandle::position() {
    std::ostringstream os;
    os << "DataHandle::position() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::rewind() {
    std::ostringstream os;
    os << "DataHandle::rewind() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Offset DataHandle::seek(const Offset& from) {
    std::ostringstream os;
    os << "DataHandle::seek(" << from << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

bool DataHandle::canSeek() const {
    std::ostringstream os;
    os << "DataHandle::canSeek() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Length DataHandle::estimate() {
    return 0;
}

Length DataHandle::openForRead() {
    std::ostringstream os;
    os << "DataHandle::openForRead() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::openForWrite(const Length&) {
    std::ostringstream os;
    os << "DataHandle::openForWrite() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::openForAppend(const Length&) {
    std::ostringstream os;
    os << "DataHandle::openForAppend() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

long DataHandle::read(void*, long) {
    std::ostringstream os;
    os << "DataHandle::read() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

long DataHandle::write(const void*, long) {
    std::ostringstream os;
    os << "DataHandle::write() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::close() {
    std::ostringstream os;
    os << "DataHandle::close() [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

Length DataHandle::size() {
    std::ostringstream oss;
    oss << "DataHandle::size() [" << *this << "]";
    throw NotImplemented(oss.str(), Here());
}

void DataHandle::skip(const Length& len) {
    seek(position() + len);
}

void DataHandle::restartReadFrom(const Offset& from) {
    std::ostringstream os;
    os << "DataHandle::restartReadFrom(" << from << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::restartWriteFrom(const Offset& offset) {
    std::ostringstream os;
    os << "DataHandle::restartWriteFrom(" << offset << ") [" << *this << "]";
    throw NotImplemented(os.str(), Here());
}

const std::set<std::string>& DataHandle::requiredMoverAttributes() const {
    static std::set<std::string> nullSet;
    return nullSet;
}

void DataHandle::toLocal(Stream& s) const {
    s << *this;
}

DataHandle* DataHandle::toLocal() {
    return this;
}

void DataHandle::toRemote(Stream& s) const {
    s << *this;
}

void DataHandle::cost(std::map<std::string, Length>& c, bool read) const {}

DataHandle* DataHandle::clone() const {
    std::ostringstream os;
    os << "DataHandle::clone(" << *this << ")";
    throw NotImplemented(os.str(), Here());
}

void DataHandle::hash(MD5& md5) const {
    std::ostringstream os;
    os << "DataHandle::hash(" << *this << ")";
    throw NotImplemented(os.str(), Here());
}


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit

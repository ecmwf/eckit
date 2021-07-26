/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/DblBuffer.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/Progress.h"
#include "eckit/log/Timer.h"
#include "eckit/runtime/Metrics.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/value/Value.h"


namespace eckit {


class DblBufferError : public Exception {
public:
    DblBufferError(const std::string& what) { reason(std::string("Double buffer error: ") + what); }
};


struct OneBuffer {
    MutexCond cond_;
    bool full_;
    long length_;
    char* buffer_;
    OneBuffer() :
        full_(false), length_(0), buffer_(0) {}
};

class DblBufferTask : public Thread {
    DblBuffer& owner_;
    DataHandle& out_;
    Length estimate_;
    OneBuffer* buffers_;
    long parent_;

public:
    DblBufferTask(DataHandle&, DblBuffer&, OneBuffer*, const Length&, long parent);
    virtual void run();
};

DblBuffer::DblBuffer(long count, long size, TransferWatcher& watcher) :
    count_(count), bufSize_(size), error_(false), restart_(false), restartFrom_(0), watcher_(watcher) {
    Log::info() << "Double buffering: " << count_ << " buffers of " << Bytes(size) << " is " << Bytes(count * size)
                << std::endl;
}

DblBuffer::~DblBuffer() {}

inline void DblBuffer::error(const std::string& why) {
    AutoLock<Mutex> lock(mutex_);
    error_ = true;
    why_   = why;
}

inline bool DblBuffer::error() {
    AutoLock<Mutex> lock(mutex_);
    return error_;
}

void DblBuffer::restart(RestartTransfer& retry) {
    AutoLock<Mutex> lock(mutex_);
    Log::warning() << "Retrying transfer from " << retry.from() << " (" << Bytes(retry.from()) << ")" << std::endl;
    error_       = true;
    restart_     = true;
    restartFrom_ = retry.from();
}

Length DblBuffer::copy(DataHandle& in, DataHandle& out) {

    Timer timer("Double buffer");
    in.compress();


    Length estimate = in.openForRead();
    AutoClose c1(in);
    watcher_.fromHandleOpened();
    out.openForWrite(estimate);
    AutoClose c2(out);
    watcher_.toHandleOpened();

    Length total  = estimate;
    Length copied = 0;

    bool more = true;
    while (more) {
        more = false;
        try {
            copied = copy(in, out, estimate);
            Log::info() << "Copied: " << copied << ", estimate: " << estimate << std::endl;
            if (estimate) {
                if (copied != estimate) {
                    std::ostringstream os;
                    os << "DblBuffer::copy(), copied: " << copied << ", estimate: " << estimate;
                    throw SeriousBug(os.str());
                }
                ASSERT(copied == estimate);
            }
        }
        catch (RestartTransfer& retry) {
            Log::warning() << "Retrying transfer from " << retry.from() << " (" << Bytes(retry.from()) << ")"
                           << std::endl;
            in.restartReadFrom(retry.from());
            out.restartWriteFrom(retry.from());
            estimate = total - retry.from();
            more     = true;
        }
    }

    Log::info() << "Transfer rate " << Bytes(estimate, timer) << std::endl;
    return copied;
}

Length DblBuffer::copy(DataHandle& in, DataHandle& out, const Length& estimate) {
    Buffer bigbuf(count_ * bufSize_);

    OneBuffer* buffers = new OneBuffer[count_];

    char* addr = bigbuf;
    for (int j = 0; j < count_; j++) {
        buffers[j].buffer_ = addr;
        addr += bufSize_;
    }

    Progress progress("Reading data", 0, estimate);

    error_   = false;
    inBytes_ = outBytes_ = 0;

    ThreadControler thread(new DblBufferTask(out, *this, buffers, estimate, Monitor::instance().self()), false);

    thread.start();

    int i = 0;

    Timer reader("Double buffer reader");
    double rate  = 0.;
    double first = 0.;


    watcher_.watch(nullptr, 0);

    while (!error()) {
        Log::message() << "Wait " << i << std::endl;
        AutoLock<MutexCond> lock(buffers[i].cond_);

        while (buffers[i].full_)
            buffers[i].cond_.wait();

        if (error())
            break;

        Log::message() << "Read " << i << std::endl;
        try {
            double x           = reader.elapsed();
            buffers[i].length_ = in.read(buffers[i].buffer_, bufSize_);
            double s           = reader.elapsed() - x;
            Log::status() << Bytes(estimate) << " at " << Bytes(buffers[i].length_ / s) << "/s" << std::endl;
            rate += s;
            if (first == 0.)
                first = rate;

            watcher_.watch(buffers[i].buffer_, buffers[i].length_);
        }
        catch (RestartTransfer& retry) {
            Log::warning() << "RestartTransfer: Exiting reader thread" << std::endl;
            buffers[i].length_ = -1;
            restart(retry);
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
            buffers[i].length_ = -1;
            error(e.what());
        }
        Log::message() << "" << std::endl;

        buffers[i].full_ = true;

        if (buffers[i].length_ == 0) {
            buffers[i].cond_.signal();
            break;
        }

        if (buffers[i].length_ < 0) {
            ASSERT(error());
            Log::warning() << "Read error... " << why_ << std::endl;
            buffers[i].cond_.signal();
            break;
        }

        inBytes_ += buffers[i].length_;
        progress(inBytes_);

        buffers[i].cond_.signal();

        i++;
        i %= count_;
    }

    Log::info() << "Read done " << Bytes(inBytes_) << std::endl;
    Log::info() << "Read rate " << Bytes(inBytes_ / rate) << "/s" << std::endl;
    if (first != rate)
        Log::info() << "Read rate no mount " << Bytes(inBytes_ / (rate - first)) << "/s" << std::endl;

    thread.wait();
    delete[] buffers;

    if (error_) {
        if (restart_)
            throw RestartTransfer(restartFrom_);
        throw DblBufferError(why_);
    }

    PANIC(inBytes_ != outBytes_);

    in.collectMetrics("source");
    out.collectMetrics("target");
    Metrics::set("size", inBytes_);
    Metrics::set("read_time", rate);
    Metrics::set("time", reader.elapsed());

    return inBytes_;
}

DblBufferTask::DblBufferTask(DataHandle& out, DblBuffer& owner, OneBuffer* buffers, const Length& estimate,
                             long parent) :
    Thread(false), owner_(owner), out_(out), estimate_(estimate), buffers_(buffers), parent_(parent) {}

void DblBufferTask::run() {
    Monitor::instance().parent(parent_);

    Log::status() << "Double buffering " << Bytes(estimate_) << std::endl;
    Progress progress("Writing data", 0, estimate_);

    int i = 0;
    Timer writer("Double buffer writer");
    double rate  = 0;
    double first = 0;

    while (!owner_.error()) {
        Log::message() << "Wait " << i << std::endl;
        AutoLock<MutexCond> lock(buffers_[i].cond_);

        while (!buffers_[i].full_)
            buffers_[i].cond_.wait();

        if (owner_.error())
            break;

        if (buffers_[i].length_ == 0)
            break;

        long length = -1;

        Log::message() << "Write " << i << std::endl;
        try {
            double x = writer.elapsed();
            length   = out_.write(buffers_[i].buffer_, buffers_[i].length_);
            double s = writer.elapsed() - x;
            Log::status() << Bytes(buffers_[i].length_ / s) << "/s" << std::endl;
            rate += s;
            if (first == 0)
                first = rate;

            ASSERT(length == buffers_[i].length_);
        }
        catch (RestartTransfer& retry) {
            Log::warning() << "RestartTransfer: Exiting writer thread" << std::endl;
            length = -1;
            owner_.restart(retry);
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
            length = -1;
            owner_.error(e.what());
        }
        Log::message() << "" << std::endl;

        buffers_[i].full_ = false;

        if (length < 0) {
            ASSERT(owner_.error());
            buffers_[i].cond_.signal();
            break;
        }

        ASSERT(length == buffers_[i].length_);

        owner_.outBytes_ += length;
        progress(owner_.outBytes_);

        buffers_[i].cond_.signal();

        i++;
        i %= owner_.count_;
    }

    Log::info() << "Write done " << Bytes(owner_.outBytes_) << std::endl;
    Log::info() << "Write rate " << Bytes(owner_.outBytes_ / rate) << "/s" << std::endl;
    if (rate != first)
        Log::info() << "Write rate no mount " << Bytes(owner_.outBytes_ / (rate - first)) << "/s" << std::endl;

    Metrics::set("write_time", rate);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

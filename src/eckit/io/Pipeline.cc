/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/FileDescHandle.h"
#include "eckit/io/Pipeline.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/Progress.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


class PipelineReader : public Thread {
    Pipeline& owner_;
    DataHandle& in_;
    DataHandle& out_;

public:
    PipelineReader(Pipeline& owner_, DataHandle& in, DataHandle& out);
    virtual void run();
};


PipelineReader::PipelineReader(Pipeline& owner, DataHandle& in, DataHandle& out) :
    owner_(owner), in_(in), out_(out) {}

void PipelineReader::run() {
    try {
        in_.saveInto(out_, owner_.watcher_);
    }
    catch (std::exception& e) {
        owner_.error(e.what());
    }
}


class PipelineExecutor : public Thread {
    Pipeline& owner_;
    DataHandle& in_;
    DataHandle& out_;

public:
    PipelineExecutor(Pipeline& owner_, DataHandle& in, DataHandle& out);
    virtual void run();
};

PipelineExecutor::PipelineExecutor(Pipeline& owner, DataHandle& in, DataHandle& out) :
    owner_(owner), in_(in), out_(out) {}


void PipelineExecutor::run() {
    try {
        owner_.execute(in_, out_);
    }
    catch (std::exception& e) {
        owner_.error(e.what());
    }
}

Pipeline::Pipeline(TransferWatcher& watcher) :

    error_(false), watcher_(watcher) {}

Pipeline::~Pipeline() {}

inline void Pipeline::error(const std::string& why) {
    AutoLock<Mutex> lock(mutex_);
    error_ = true;
    why_   = why;
}

inline bool Pipeline::error() {
    AutoLock<Mutex> lock(mutex_);
    return error_;
}

Length Pipeline::copy(DataHandle& in, DataHandle& out) {
    int reader[2];
    SYSCALL(::pipe(reader));

    FileDescHandle reader_out(reader[1], true);
    FileDescHandle reader_in(reader[0], true);


    int process[2];
    SYSCALL(::pipe(process));

    FileDescHandle process_out(process[1], true);
    FileDescHandle process_in(process[0], true);


    ThreadControler thread1(new PipelineReader(*this, in, reader_out), false);
    thread1.start();

    ThreadControler thread2(new PipelineExecutor(*this, reader_in, process_out), false);
    thread2.start();

    Length total = 0;

    try {
        total = process_in.saveInto(out, watcher_);
    }
    catch (std::exception& e) {
        error(e.what());
    }

    try {
        thread1.wait();
    }
    catch (std::exception& e) {
        error(e.what());
    }

    try {
        thread2.wait();
    }
    catch (std::exception& e) {
        error(e.what());
    }

    if (error_) {
        throw SeriousBug(why_);
    }

    return total;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

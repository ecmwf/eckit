/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <iomanip>
#include <memory>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/container/MappedArray.h"
#include "eckit/container/SharedMemArray.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/log/Log.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/transaction/TxnLog.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


class TxnArray : private eckit::NonCopyable {

public:

    typedef TxnID* iterator;
    typedef const TxnID* const_iterator;

    virtual ~TxnArray() {}

    virtual void lock()   = 0;
    virtual void unlock() = 0;

    virtual unsigned long size()               = 0;
    virtual TxnID& operator[](unsigned long n) = 0;
};

class MemoryMappedTxnArray : public TxnArray {

    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual unsigned long size() { return map_.size(); }
    virtual TxnID& operator[](unsigned long n) { return map_[n]; }

    MappedArray<TxnID> map_;

public:

    MemoryMappedTxnArray(const PathName& path, unsigned long size) : TxnArray(), map_(path, size) {}
};

class SharedMemoryTxnArray : public TxnArray {

    virtual void lock() { map_.lock(); }
    virtual void unlock() { map_.unlock(); }

    virtual unsigned long size() { return map_.size(); }
    virtual TxnID& operator[](unsigned long n) { return map_[n]; }

    SharedMemArray<TxnID> map_;

public:

    SharedMemoryTxnArray(const PathName& path, const std::string& name, unsigned long size) :
        TxnArray(), map_(path, name, size) {}
};


//----------------------------------------------------------------------------------------------------------------------

template <class T>
PathName TxnLog<T>::buildPath(const std::string& name) {
    PathName path = std::string("~/txn/") + name;
    path.mkdir();
    return path;
}

template <class T>
TxnLog<T>::TxnLog(const std::string& name) : path_(buildPath(name)), next_(path_ + "/next") {
    std::string txnArrayType = Resource<std::string>("txnArrayType", "MemoryMapped");

    if (txnArrayType == "MemoryMapped") {
        nextID_ = new MemoryMappedTxnArray(next_, 1);
    }
    else if (txnArrayType == "SharedMemory") {
        std::string s = path_;
        std::replace(s.begin(), s.end(), '/', '-');
        nextID_ = new SharedMemoryTxnArray(next_, s, 1);
    }
    else {
        std::ostringstream oss;
        oss << "Invalid txnArrayType : " << txnArrayType << ", valid types are 'MemoryMapped' and 'SharedMemory'"
            << std::endl;
        throw eckit::BadParameter(oss.str(), Here());
    }


    AutoLock<TxnArray> lock(*nextID_);
    Log::debug() << "TxnLog file is " << path_ << std::endl;

    PathName done = path_ + "/done";
    done.mkdir();
}

template <class T>
TxnLog<T>::~TxnLog() {}

template <class T>
PathName TxnLog<T>::name(const T& event) {
    std::ostringstream s;
    s << std::setfill('0') << std::setw(10) << event.transactionID();
    return path_ + "/" + s.str();
}


template <class T>
void TxnLog<T>::begin(T& event) {
    AutoLock<TxnArray> lock(*nextID_);

    if (event.transactionID() == 0)
        event.transactionID(++(*nextID_)[0]);

    PathName path = name(event);
    ASSERT(!path.exists());

    FileStream log(path, "w");
    auto c = closer(log);
    log << event;
}

template <class T>
void TxnLog<T>::update(const T& event) {
    // AutoLock<TxnArray > lock(*nextID_);

    PathName path = name(event);
    PathName next = path + ".tmp";
    {
        FileStream log(next, "w");
        auto c = closer(log);
        log << event;
    }
    PathName::rename(next, path);
}

template <class T>
void TxnLog<T>::end(T& event, bool backup) {
    AutoLock<TxnArray> lock(*nextID_);

    PathName path = name(event);

    if (backup) {
        std::ostringstream s;
        s << path.dirName() << "/done/" << TimeStamp(time(0), "%Y%m%d");

        // Append to current day's backup

        FileStream log(s.str(), "a");
        auto c = closer(log);
        log << event;
    }

    // Remove file

    path.unlink();
}

template <class T>
bool TxnLog<T>::exists(T& event) {
    PathName path = name(event);
    return path.exists();
}

template <class T>
class RecoverThread : public Thread {
    TxnArray& nextID_;
    TxnRecoverer<T>& client_;
    std::vector<PathName> result_;
    long age_;
    time_t now_;
    virtual void run();

public:

    RecoverThread(const PathName&, TxnArray&, TxnRecoverer<T>&, long);
    void recover();
};

template <class T>
RecoverThread<T>::RecoverThread(const PathName& path, TxnArray& nextID, TxnRecoverer<T>& client, long age) :
    nextID_(nextID), client_(client), age_(age), now_(::time(0)) {
    AutoLock<TxnArray> lock(nextID_);
    PathName::match(path + "/[0-9]*", result_);

    // Sort by ID to preserve order
    std::sort(result_.begin(), result_.end());
    Log::info() << result_.size() << " task(s) found in log files" << std::endl;

    if (result_.size()) {
        PathName last    = result_[result_.size() - 1];
        std::string name = last.baseName();
        TxnID id         = Translator<std::string, unsigned long long>()(name);
        if (id >= nextID_[0])
            nextID_[0] = id + 1;
    }
}

template <class T>
void RecoverThread<T>::run() {
    Monitor::instance().name("recover");
    recover();
}

template <class T>
void RecoverThread<T>::recover() {
    for (Ordinal i = 0; i < result_.size(); i++) {

        if (now_ - result_[i].created() < age_)
            Log::info() << "Skipping " << result_[i] << ", created " << Seconds(now_ - result_[i].created()) << " ago."
                        << std::endl;
        else
            try {
                FileStream log(result_[i], "r");
                auto c  = closer(log);
                T* task = Reanimator<T>::reanimate(log);
                if (task) {
                    ASSERT(task->transactionID() < nextID_[0]);
                    client_.push(task);
                }
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
    }
}

template <class T>
void TxnLog<T>::recover(TxnRecoverer<T>& client, bool inThread, long age) {
    if (inThread) {
        ThreadControler c(new RecoverThread<T>(path_, *nextID_, client, age));
        c.start();
    }
    else {
        RecoverThread<T> r(path_, *nextID_, client, age);
        r.recover();
    }
}

template <class T>
void TxnLog<T>::find(TxnFinder<T>& r) {

    // Look for active transactions

    if (r.active()) {

        PathName path = path_ + "/[0-9]*";
        std::vector<PathName> active;

        PathName::match(path, active);

        LOG_DEBUG_LIB(LibEcKit) << "TxnLog found active: " << active << std::endl;

        for (Ordinal j = 0; (j < active.size()); ++j) {
            try {
                FileStream log(active[j], "r");
                auto c = closer(log);
                std::unique_ptr<T> task(Reanimator<T>::reanimate(log));
                if (task) {
                    LOG_DEBUG_LIB(LibEcKit)
                        << "Task found - id: " << task->transactionID() << " task: " << *task << std::endl;
                    if (r.found(*task)) {
                        return;
                    }
                }
            }
            catch (Abort& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is re-thrown" << std::endl;
                throw;
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }
    }


    // Look for non-active transactions

    if (r.old()) {

        PathName path = path_ + "/done/[0-9]*";
        std::vector<PathName> dates;

        PathName::match(path, dates);

        // Sort by date in reverse order
        std::sort(dates.begin(), dates.end(), std::greater<PathName>());

        LOG_DEBUG_LIB(LibEcKit) << "TxnLog found dates: " << dates << std::endl;

        for (Ordinal k = 0; k < dates.size(); k++) {
            Log::info() << "Searching " << dates[k] << std::endl;
            try {
                FileStream log(dates[k], "r");
                auto c = closer(log);
                std::unique_ptr<T> task(Reanimator<T>::reanimate(log));
                while (task) {
                    LOG_DEBUG_LIB(LibEcKit)
                        << "Task found - id: " << task->transactionID() << " task: " << *task << std::endl;
                    if (r.found(*task)) {
                        return;
                    }
                    task.reset(Reanimator<T>::reanimate(log));
                }
            }
            catch (Abort& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is re-thrown" << std::endl;
                throw;
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

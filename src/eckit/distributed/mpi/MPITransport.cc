/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/distributed/mpi/MPITransport.h"

#include <unistd.h>

#include <iostream>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Statistics.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/mpi/Comm.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/os/AutoAlarm.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"

#include "eckit/distributed/Actor.h"
#include "eckit/distributed/Message.h"


namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

MPITransport::MPITransport(const eckit::option::CmdArgs& args) :
    Transport(args),
    comm_(eckit::mpi::comm("world")) {


    // eckit::mpi::createCo5mm("atlas", eckit::mpi::comm().self());
    // LibAtlas::setComm("atlas");

    eckit::mpi::setCommDefault("self");  // for Atlas

    rank_       = comm_.rank();
    totalRanks_ = comm_.size();

    eckit::Main::instance().taskID(rank_);

    hostname_ = comm_.processorName();

    size_t writers = 0;
    args.get("writers", writers);

    // Assuming aprun is run with --cc cpu, the taks are distributed evenly on each nodes,
    // starting from 0 to totalRanks_ - 1

    if (writers) {
        int n = totalRanks_ / writers;
        int r = totalRanks_ - 1;
        ASSERT(n > 1);

        int w = 1;
        for (size_t i = 0; i < writers; ++i) {

            if (r <= 0) {
                r = 1;
            }
            ASSERT(ranksToWriters_.find(r) == ranksToWriters_.end());
            ranksToWriters_[r] = w;

            ASSERT(writersToRanks_.find(w) == writersToRanks_.end());
            writersToRanks_[w] = r;

            w++;
            r -= n;
        }

        // eckit::Log::info() << "Writers are: " << ranksToWriters_ << " " << writersToRanks_ << std::endl;
    }
    // eckit::Log::info() << " starting " << std::endl;


    std::ostringstream oss;
    if (rank_) {
        if (ranksToWriters_.find(rank_) != ranksToWriters_.end()) {
            oss << "Writer-" << rank_ << "@" << hostname_;
        }
        else {
            oss << "Worker-" << rank_ << "@" << hostname_;
        }
    }
    else {
        oss << "Producer-0@" << hostname_;
    }


    title_ = oss.str();

    std::ostringstream oid;
    oid << rank_;
    id_ = oid.str();


    std::string base;
    if (args.get("split-logs", base)) {


        std::ostringstream fname;
        fname << base << '.' << rank_;

        if (rank_ == 0) {
            eckit::Log::addFile(fname.str());
        }
        else {
            eckit::Log::setFile(fname.str());
        }
    }

    std::string hostname = eckit::Main::hostname();

    eckit::Log::info() << "Start of "
                       << title_
                       << " host: " << hostname
                       << " pid: " << ::getpid()
                       << std::endl;
}

MPITransport::~MPITransport() {
}

bool MPITransport::single() const {
    return totalRanks_ == 1;
}

void MPITransport::initialise() {
}

void MPITransport::abort() {
    eckit::AutoAlarm alarm(10);
    // eckit::Log::info() << " calling MPI abort" << std::endl;

    comm_.abort(1);
}

bool MPITransport::producer() const {
    return rank_ == 0;
}

bool MPITransport::writer() const {
    return ranksToWriters_.find(rank_) != ranksToWriters_.end();
}

void MPITransport::sendMessageToNextWorker(const Message& msg) {

    int worker = comm_.anySource();
    int tag    = comm_.anyTag();

    Message request;

    receive(request, worker, tag);

    ASSERT(ranksToWriters_.find(worker) == ranksToWriters_.end());
    ASSERT(tag == Actor::READY);

    send(msg, worker, msg.tag());
}

void MPITransport::getNextWorkMessage(Message& message) {

    int master = 0;
    send(Message::readyMessage(), master, Actor::READY);


    int source = master;
    int tag    = comm_.anyTag();

    receive(message, source, tag);

    ASSERT(source == master);
    ASSERT(tag == Actor::WORK || tag == Actor::SHUTDOWN);

    message.rewind();
    message.messageReceived(tag, source);
}

void MPITransport::print(std::ostream& out) const {
    out << "MPITransport[" << title_ << "]";
}

void MPITransport::synchronise() {
    // eckit::Log::info() << " Calling MPI_Barrier " << std::endl;
    eckit::AutoTiming timing(statistics_.barrierTiming_);
    comm_.barrier();
}

void MPITransport::sendStatisticsToProducer(const Message& message) {
    int producer = 0;
    send(message, producer, Actor::STATISTICS);
}

void MPITransport::sendToWriter(size_t writer, const Message& message) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    auto j = writersToRanks_.find(writer);
    ASSERT(j != writersToRanks_.end());
    send(message, (*j).second, message.tag());
}

void MPITransport::getNextWriteMessage(Message& message) {
    // eckit::Log::info()
    //           << " getNextWriteMessage"
    //           << std::endl;

    int source = comm_.anySource();
    int tag    = comm_.anyTag();

    receive(message, source, tag);

    ASSERT(ranksToWriters_.find(source) == ranksToWriters_.end());


    message.rewind();
    message.messageReceived(tag, source);

    ASSERT(tag == Actor::WRITE
           || tag == Actor::OPEN
           || tag == Actor::CLOSE
           || tag == Actor::SHUTDOWN);
}

void MPITransport::send(const Message& message, int target, int tag) {
    eckit::AutoTiming timing(statistics_.sendTiming_);
    ASSERT(message.messageSize() > 0);

    comm_.send(const_cast<void*>(message.messageData()), message.messageSize(), target, tag);

    statistics_.sendCount_++;
    statistics_.sendSize_ += message.messageSize();
}

void MPITransport::synchronisedSend(const Message& message, int target, int tag) {
    eckit::AutoTiming timing(statistics_.sendTiming_);

    ASSERT(message.messageSize() > 0);

    comm_.synchronisedSend(const_cast<void*>(message.messageData()), message.messageSize(), target, tag);

    statistics_.sendCount_++;
    statistics_.sendSize_ += message.messageSize();
}

void MPITransport::receive(Message& message, int& source, int& tag) {
    eckit::AutoTiming timing(statistics_.receiveTiming_);

    eckit::mpi::Status status = comm_.probe(source, tag);

    size_t size = comm_.getCount<void>(status);

    message.reserve(size);

    statistics_.receiveCount_++;
    statistics_.receiveSize_ += size;

    source = status.source();
    tag    = status.tag();

    status = comm_.receive(message.messageData(), message.bufferSize(), source, tag);
}

void MPITransport::sendShutDownMessage(const Actor& actor) {
    eckit::AutoTiming timing(statistics_.shutdownTiming_);

    // Shutdown workers
    size_t count = totalRanks_ - ranksToWriters_.size() - 1;

    eckit::Log::info()
        << " shutdown workers count="
        << count
        << std::endl;

    std::set<int> r;

    for (int i = 1; i < totalRanks_; i++) {
        r.insert(i);
    }

    while (count > 0) {
        int worker = comm_.anySource();
        int tag    = comm_.anyTag();

        Message message;

        receive(message, worker, tag);

        ASSERT(ranksToWriters_.find(worker) == ranksToWriters_.end());

        switch (tag) {
            case Actor::READY:
                eckit::Log::info()
                    << " shutdown worker="
                    << worker
                    << " left=" << count
                    << std::endl;
                send(Message::shutdownMessage(), worker, Actor::SHUTDOWN);
                break;

            case Actor::STATISTICS:
                actor.messageFromWorker(message, worker);
                r.erase(worker);
                count--;
                eckit::Log::info()
                    << " stats from worker="
                    << worker
                    << " left=" << count
                    << " " << r
                    << std::endl;
                break;

            default:
                ASSERT(tag == Actor::READY || tag == Actor::STATISTICS);
                break;
        }
    }

    count = ranksToWriters_.size();

    eckit::Log::info()
        << " shutdown workers count="
        << count
        << std::endl;

    for (auto j = writersToRanks_.begin(); j != writersToRanks_.end(); ++j) {
        send(Message::shutdownMessage(), (*j).second, Actor::SHUTDOWN);
    }

    while (count > 0) {
        int writer = comm_.anySource();
        int tag    = comm_.anyTag();

        Message message;

        receive(message, writer, tag);

        ASSERT(ranksToWriters_.find(writer) != ranksToWriters_.end());

        switch (tag) {

            case Actor::STATISTICS:
                actor.messageFromWriter(message, writer);
                r.erase(writer);
                count--;
                eckit::Log::info()
                    << " stats from writer="
                    << writer
                    << " left=" << count
                    << r
                    << std::endl;
                break;

            default:
                ASSERT(tag == Actor::STATISTICS);
                break;
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------

static TransportBuilder<MPITransport> builder("mpi");

}  // namespace eckit

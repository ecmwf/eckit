/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel


#pragma once

#include "eckit/distributed/Transport.h"

#include <list>

#include "eckit/mpi/Comm.h"
#include "eckit/thread/Mutex.h"


namespace eckit::option {
class Option;
class CmdArgs;
}  // namespace eckit::option

namespace eckit::distributed {

class Message;

//----------------------------------------------------------------------------------------------------------------------

class MPITransport : public Transport {
public:  // methods

    MPITransport(const eckit::option::CmdArgs& args);
    virtual ~MPITransport() override;

private:  // methods

    virtual void sendMessageToNextWorker(const Message& message) override;
    virtual void getNextWorkMessage(Message& message) override;
    virtual void sendStatisticsToProducer(const Message& message) override;
    virtual void sendShutDownMessage(const Actor&) override;

    virtual bool producer() const override;
    virtual bool single() const override;
    virtual void initialise() override;
    virtual void abort() override;
    virtual void synchronise() override;
    virtual bool writer() const override;
    virtual void sendToWriter(size_t writer, const Message& message) override;
    virtual void getNextWriteMessage(Message& message) override;

    void print(std::ostream& out) const override;

    void send(const Message& message, int target, int tag);
    void receive(Message& message, int& source, int& tag);
    void synchronisedSend(const Message& message, int target, int tag);

private:  // members

    int totalRanks_;
    int rank_;

    std::map<int, int> writersToRanks_;
    std::map<int, int> ranksToWriters_;

    std::string hostname_;

    mutable eckit::Mutex mutex_;

    eckit::mpi::Comm& comm_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

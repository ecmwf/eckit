// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   NoTransport.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_NoTransport_H
#define eckit_NoTransport_H


#include "eckit/distributed/Transport.h"


namespace eckit::option {
class Option;
class CmdArgs;
}  // namespace eckit::option

namespace eckit::distributed {

class Message;

//----------------------------------------------------------------------------------------------------------------------

class NoTransport : public Transport {
public:  // methods

    NoTransport(const eckit::option::CmdArgs& args);
    virtual ~NoTransport() override;

protected:  // methods


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


protected:  // members
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif

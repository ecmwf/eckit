/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

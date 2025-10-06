/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   TCPTransport.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_TCPTransport_H
#define eckit_TCPTransport_H

#include <memory>
#include <vector>

#include "eckit/io/Select.h"
#include "eckit/net/TCPServer.h"

#include "eckit/distributed/Transport.h"

namespace eckit {
class Stream;
}

namespace eckit::option {
class Option;
class CmdArgs;
}  // namespace eckit::option

namespace eckit::distributed {

class Message;

//----------------------------------------------------------------------------------------------------------------------

class Connection;

class TCPTransport : public Transport {
public:  // methods

    TCPTransport(const eckit::option::CmdArgs& args);
    virtual ~TCPTransport() override;

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


private:

    // typedef bool (TCPTransport::*)(Connection&);

    // void all(bool (TCPTransport ::* const)(Connection&));

    void disconnect() const;
    void disconnect(Connection&) const;
    void disconnect(std::exception& e, Connection&) const;
    Connection& producerConnection() const;

private:  // members

    void accept();
    void connect();

    bool send(const Message& message);
    void cleanup();

    mutable std::unique_ptr<Connection> producer_;

    mutable std::unique_ptr<eckit::net::TCPServer> accept_;

    mutable std::vector<Connection*> connections_;

    mutable eckit::Select select_;

    mutable size_t nextId_;

    bool master_;
    bool worker_;
    bool writer_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif

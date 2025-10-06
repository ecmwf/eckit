/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Transport.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#pragma once

#include <iosfwd>
#include <string>

#include "eckit/distributed/TransportStatistics.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit::option {
class Option;
class CmdArgs;
}  // namespace eckit::option

namespace eckit::distributed {

class Actor;
class Message;

//----------------------------------------------------------------------------------------------------------------------

class Transport : private eckit::NonCopyable {
public:  // methods

    Transport(const eckit::option::CmdArgs& args);
    virtual ~Transport();

    virtual void sendMessageToNextWorker(const Message& message)  = 0;
    virtual void getNextWorkMessage(Message& message)             = 0;
    virtual void sendStatisticsToProducer(const Message& message) = 0;

    virtual void sendToWriter(size_t writer, const Message& message) = 0;
    virtual void getNextWriteMessage(Message& message)               = 0;

    virtual void sendShutDownMessage(const Actor&) = 0;

    virtual void initialise()  = 0;
    virtual void abort()       = 0;
    virtual void synchronise() = 0;

    virtual bool producer() const = 0;
    virtual bool single() const   = 0;
    virtual bool writer() const   = 0;

    virtual const std::string& title() const;
    virtual const std::string& id() const;

    const TransportStatistics& statistics() const;

protected:

    std::string title_;
    std::string id_;

    TransportStatistics statistics_;

private:  // methods

    virtual void print(std::ostream& out) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Transport& x) {
        x.print(s);
        return s;
    }
};


class TransportFactory {
    std::string name_;
    virtual Transport* make(const eckit::option::CmdArgs& args) = 0;

protected:

    TransportFactory(const std::string&);
    virtual ~TransportFactory();

public:

    static Transport* build(const eckit::option::CmdArgs& args);
    static void list(std::ostream&);
};


template <class T>
class TransportBuilder : public TransportFactory {
    virtual Transport* make(const eckit::option::CmdArgs& args) override { return new T(args); }

public:

    TransportBuilder(const std::string& name) : TransportFactory(name) {}
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/TCPHandle.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_filesystem_TCPHandle_h
#define eckit_filesystem_TCPHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPClient.h"


namespace eckit {


class TCPHandle : public DataHandle {
public:
    // -- Contructors

    TCPHandle(Stream&);
    TCPHandle(const std::string& host, int port);

    // -- Destructor

    ~TCPHandle();

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void rewind() override;

    virtual DataHandle* clone() const override;

    virtual void print(std::ostream&) const override;
    virtual std::string title() const override;
    virtual bool moveable() const override { return true; }

    virtual bool canSeek() const override { return false; }

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

protected:
    std::string host_;
    int port_;
    net::TCPClient connection_;

private:
    static ClassSpec classSpec_;
    static Reanimator<TCPHandle> reanimator_;
};


}  // namespace eckit

#endif

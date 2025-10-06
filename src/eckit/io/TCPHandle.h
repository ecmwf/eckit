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

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void rewind() override;

    DataHandle* clone() const override;

    void print(std::ostream&) const override;
    std::string title() const override;
    bool moveable() const override { return true; }

    bool canSeek() const override { return false; }

    virtual void selectMover(eckit::MoverTransferSelection&, bool) const override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

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

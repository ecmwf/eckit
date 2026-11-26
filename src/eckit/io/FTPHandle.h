// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File io/FTPHandle.h
// Manuel Fuentes - ECMWF May 96

#ifndef eckit_filesystem_FTPHandle_h
#define eckit_filesystem_FTPHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPSocket.h"


namespace eckit {


class FTPHandle : public DataHandle {
public:

    // -- Exceptions

    class FTPError : public std::exception {
        virtual const char* what() const noexcept;
    };

    // -- Contructors

    FTPHandle(const std::string&, const std::string&, int port = 21);
    FTPHandle(Stream&);

    // -- Destructor

    ~FTPHandle() {}

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void rewind() override;
    void print(std::ostream&) const override;
    bool canSeek() const override { return false; }

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    std::string remote_;
    std::string host_;
    int port_;
    net::TCPClient cmds_;
    net::TCPSocket data_;

    // -- Methods

    void ftpCommand(const std::string&);
    std::string readLine();
    void open(const std::string&);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<FTPHandle> reanimator_;
};


}  // namespace eckit

#endif

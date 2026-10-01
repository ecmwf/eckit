// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File io/MultiSocketHandle.h
// Baudouin Raoult - ECMWF Apr 21

#ifndef eckit_filesystem_MultiSocketHandle_h
#define eckit_filesystem_MultiSocketHandle_h

#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/net/MultiSocket.h"


namespace eckit {


class MultiSocketHandle : public DataHandle {
public:

    // -- Contructors

    MultiSocketHandle(Stream&);
    MultiSocketHandle(const std::string& host, int port, size_t streams, size_t messageSize, size_t bufferSize = 0);

    // -- Destructor

    ~MultiSocketHandle();

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

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

protected:

    std::string host_;
    int port_ = 0;
    std::unique_ptr<net::MultiSocket> connection_;
    size_t streams_     = 0;
    size_t messageSize_ = 0;
    size_t bufferSize_  = 0;

private:

    static ClassSpec classSpec_;
    static Reanimator<MultiSocketHandle> reanimator_;
};


}  // namespace eckit

#endif

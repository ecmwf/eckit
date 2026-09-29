// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   June 2017

#ifndef eckit_filesystem_URLHandle_h
#define eckit_filesystem_URLHandle_h

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class URLException : public Exception {
    int code_;

public:

    URLException(const std::string& what, int code) : Exception(what), code_(code) {}
    int code() const { return code_; }
};

//----------------------------------------------------------------------------------------------------------------------

class URLHandle : public DataHandle {
public:

    URLHandle(const std::string& uri, bool useSSL = true);

    URLHandle(Stream&);

    ~URLHandle();

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    // void rewind() override;
    void print(std::ostream&) const override;
    Length estimate() override;
    Length size() override;

    bool canSeek() const override { return false; }

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    std::string uri_;
    std::unique_ptr<DataHandle> handle_;

    bool useSSL_;

    DataHandle& handle();

    static ClassSpec classSpec_;
    static Reanimator<URLHandle> reanimator_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

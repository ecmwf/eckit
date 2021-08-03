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

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    // virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    virtual Length estimate() override;
    virtual Length size() override;

    virtual bool canSeek() const override { return false; }

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

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

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
/// @author Tiago Quintino
/// @date   Dec 2013

#ifndef eckit_filesystem_RawFileHandle_h
#define eckit_filesystem_RawFileHandle_h

#include "eckit/io/DataHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RawFileHandle : public DataHandle {
public:
    RawFileHandle(const std::string& path, bool overwrite = false);

    ~RawFileHandle();

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void print(std::ostream&) const override;

    virtual Length size() override;
    virtual Length estimate() override;
    virtual Offset position() override;
    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return true; }
    virtual void skip(const Length&) override;

    virtual void encode(Stream&) const override;

private:
    std::string path_;
    bool overwrite_;
    int fd_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

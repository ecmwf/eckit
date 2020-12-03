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
/// @date   May 1996

#ifndef eckit_io_FileHandle_h
#define eckit_io_FileHandle_h

#include <memory>

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

class FileHandle : public DataHandle {

public:
    FileHandle(const std::string&, bool = false);
    FileHandle(Stream&);

    virtual ~FileHandle() override;

    void advance(const Length&);

    const std::string& path() const { return name_; }

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    virtual Length size() override;
    virtual Length estimate() override;

    virtual Offset position() override;
    virtual bool isEmpty() const override;
    virtual void restartReadFrom(const Offset& from) override;
    virtual void restartWriteFrom(const Offset& from) override;
    virtual void toRemote(Stream&) const override;
    virtual void cost(std::map<std::string, Length>&, bool) const override;
    virtual std::string title() const override;
    virtual std::string metricsTag() const override;

    virtual bool moveable() const override { return true; }

    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override;
    virtual void skip(const Length&) override;

    virtual DataHandle* clone() const override;
    virtual void hash(MD5& md5) const override;

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:  // members
    std::string name_;
    bool overwrite_;
    FILE* file_;
    bool read_;

    std::unique_ptr<Buffer> buffer_;

private:  // methods
    void open(const char*);

    static ClassSpec classSpec_;
    static Reanimator<FileHandle> reanimator_;
};

}  // namespace eckit

#endif

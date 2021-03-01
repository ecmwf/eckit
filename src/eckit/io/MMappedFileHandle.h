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
/// @date   May 2020

#ifndef eckit_io_MMappedFileHandle_h
#define eckit_io_MMappedFileHandle_h

#include <memory>
#include "eckit/io/DataHandle.h"


namespace eckit {

class MMappedFileHandle : public DataHandle {

public:
    MMappedFileHandle(const std::string&);
    MMappedFileHandle(Stream&);

    virtual ~MMappedFileHandle() override;

    const std::string& path() const { return path_; }

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

    virtual std::string title() const override;
    virtual std::string metricsTag() const override;

    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return true; }
    virtual void skip(const Length&) override;

    virtual DataHandle* clone() const override;
    virtual void hash(MD5& md5) const override;

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:  // members
    std::string path_;


    std::unique_ptr<DataHandle> handle_;
    void* mmap_;
    int fd_;
    off_t length_;

private:  // methods
    void open(const char*);

    static ClassSpec classSpec_;
    static Reanimator<MMappedFileHandle> reanimator_;
};

}  // namespace eckit

#endif

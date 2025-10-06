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

    ~FileHandle() override;

    void advance(const Length&);

    const std::string& path() const { return name_; }

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    void rewind() override;
    void print(std::ostream&) const override;
    Length size() override;
    Length estimate() override;

    Offset position() override;
    bool isEmpty() const override;
    void restartReadFrom(const Offset& from) override;
    void restartWriteFrom(const Offset& from) override;
    void toRemote(Stream&) const override;
    void selectMover(MoverTransferSelection&, bool read) const override;

    std::string title() const override;
    std::string metricsTag() const override;

    bool moveable() const override { return true; }

    Offset seek(const Offset&) override;
    bool canSeek() const override;
    void skip(const Length&) override;

    DataHandle* clone() const override;
    void hash(MD5& md5) const override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

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

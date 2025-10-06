/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/EmptyHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_EmptyHandle_h
#define eckit_filesystem_EmptyHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class EmptyHandle : public DataHandle {
public:

    // -- Contructors
    EmptyHandle() {}
    EmptyHandle(Stream& s) : DataHandle(s) {}

    // -- Destructor

    ~EmptyHandle() {}

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override { return 0; }
    void openForWrite(const Length&) override {}
    void openForAppend(const Length&) override {}

    long read(void*, long) override { return 0; }
    long write(const void*, long n) override { return n; }
    void close() override {}
    void flush() override {}
    Offset seek(const Offset&) override { return 0; }
    Offset position() override { return 0; }
    void rewind() override {}
    void print(std::ostream& s) const override { s << "Empty Handle"; }
    bool isEmpty() const override { return true; }
    bool canSeek() const override { return true; }

    // From Streamable

    void encode(Stream& s) const override { DataHandle::encode(s); }
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<EmptyHandle> reanimator_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

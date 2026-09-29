// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

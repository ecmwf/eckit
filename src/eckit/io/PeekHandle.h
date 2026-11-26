// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File io/PeekHandle.h
// Baudouin Raoult - ECMWF May 2020

#ifndef eckit_filesystem_PeekHandle_h
#define eckit_filesystem_PeekHandle_h

#include <deque>

#include "eckit/io/HandleHolder.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PeekHandle : public DataHandle, public HandleHolder {
public:

    /// Contructor, taking ownership

    PeekHandle(DataHandle*);

    /// Contructor, not taking ownership

    PeekHandle(DataHandle&);

    /// Destructor

    ~PeekHandle() override;

    // -- Operators

    // -- Methods

    unsigned char peek(size_t);
    long peek(void* buffer, size_t size, size_t offset = 0);

    size_t peeked() const;

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;

    long read(void*, long) override;
    void close() override;

    void rewind() override;
    void print(std::ostream&) const override;
    void skip(const Length&) override;

    Offset seek(const Offset&) override;
    bool canSeek() const override;

    Length estimate() override;
    Offset position() override;
    Length size() override;

    std::string title() const override;
    void collectMetrics(const std::string& what) const override;  // Tag for metrics collection

private:  // members

    std::deque<unsigned char> peek_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

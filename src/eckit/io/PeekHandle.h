/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    virtual ~PeekHandle() override;

    // -- Operators

    // -- Methods

    unsigned char peek(size_t);
    long peek(void* buffer, size_t size, size_t offset = 0);

    size_t peeked() const;

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead() override;

    virtual long read(void*, long) override;
    virtual void close() override;

    virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    virtual void skip(const Length&) override;

    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override;

    virtual Length estimate() override;
    virtual Offset position() override;
    virtual Length size() override;

    virtual std::string title() const override;
    virtual void collectMetrics( const std::string& what) const override; // Tag for metrics collection

private:  // members
    std::deque<unsigned char> peek_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

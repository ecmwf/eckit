/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/SeekableHandle.h
// Baudouin Raoult - ECMWF May 2020

#ifndef eckit_filesystem_SeekableHandle_h
#define eckit_filesystem_SeekableHandle_h

#include <memory>

#include "eckit/io/HandleHolder.h"
#include "eckit/io/PeekHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class SeekableHandle : public DataHandle {
public:
    /// Contructor, taking ownership

    SeekableHandle(PeekHandle*);

    /// Contructor, not taking ownership

    SeekableHandle(PeekHandle&);

    /// Destructor

    virtual ~SeekableHandle() override;

    // -- Operators

    // -- Methods

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


private:  // members
    PeekHandle& handle_;
    size_t position_;

    virtual std::string title() const override;
    virtual void collectMetrics( const std::string& what) const override; // Tag for metrics collection

};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

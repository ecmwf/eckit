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

    SeekableHandle(PeekHandle*);
    SeekableHandle(PeekHandle&);

    /// Destructor

    ~SeekableHandle() override;

    // -- Operators

    // -- Methods

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

    // Jump to the end of the buffered position, and start again.
    void clear();


private:  // members

    bool owned_;
    PeekHandle* handle_;
    Offset seekableStart_;
    Offset position_;

    std::string title() const override;
    void collectMetrics(const std::string& what) const override;  // Tag for metrics collection
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

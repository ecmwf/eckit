// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File BufferCache.h
// Baudouin Raoult - (c) ECMWF Jul 11

#ifndef eckit_BufferCache_h
#define eckit_BufferCache_h

#include <cstdint>
#include <cstring>
#include <ctime>
#include <iosfwd>

#include "eckit/io/Buffer.h"


namespace eckit {

//-----------------------------------------------------------------------------

class BufferCache {
public:

    // -- Exceptions
    // None

    // -- Contructors

    BufferCache(size_t = 1024);
    BufferCache(const BufferCache&);
    BufferCache& operator=(const BufferCache&);

    // -- Destructor

    ~BufferCache();

    // -- Convertors
    // None

    // -- Operators

    bool operator<(const BufferCache& other) const;

    // -- Methods
    // None

    void add(const void*, size_t);
    void reset();

    size_t count() const { return count_; }

    const void* buffer() const { return buffer_; }

    time_t updated() const { return updated_; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed


    // -- Members

    size_t count_;
    Buffer buffer_;
    time_t updated_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const BufferCache& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

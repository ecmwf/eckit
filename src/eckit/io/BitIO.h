/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_BitIO_h
#define eckit_BitIO_h

#include <bitset>

#include "eckit/eckit.h"


//-----------------------------------------------------------------------------

namespace eckit {

class DataHandle;

//-----------------------------------------------------------------------------

class BitIO  {

public: // methods

    /// Contructor

    BitIO(DataHandle& handle);

    /// Destructor

    ~BitIO();

    // From DataHandle

    size_t count() const;

    void write(size_t code, size_t nbits);
    void flush();

//=============================

    size_t read(size_t nbits, size_t EOF_MARKER = 0);


private: // members

    DataHandle& handle_;

    unsigned long long buffer_;

    size_t used_;
    size_t count_;
    bool write_;

//   ---


// -- Class members


};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif

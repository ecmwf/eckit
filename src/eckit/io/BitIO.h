/*
 * (C) Copyright 1996- ECMWF.
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

class BitIO {

public:  // methods

    /// Contructor

    // padded: pad the last bits with zero at end-of-file
    BitIO(DataHandle& handle, bool padded = false);

    /// Destructor

    ~BitIO();

    //

    size_t bitCount() const;   // Bits written, read
    size_t byteCount() const;  // Bits written, read

    void write(size_t code, size_t nbits);

    //=============================

    // EOF_MARKER = 0: Throw an exception on end of file
    // EOF_MARKER = N: return N on end of file

    size_t read(size_t nbits, size_t EOF_MARKER = 0);
    size_t readAny(size_t& nbits);


private:  // members

    DataHandle& handle_;

    unsigned long long buffer_;

    size_t used_;
    size_t bits_;

    bool write_;
    bool eof_;
    bool padded_;
    bool opened_;

    //   ---

    void flush();

    // -- Class members
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

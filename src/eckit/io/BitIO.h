// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

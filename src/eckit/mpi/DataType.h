// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_DataType_h
#define eckit_mpi_DataType_h

#include <cstddef>
#include <vector>

#include "eckit/mpi/DataType.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm;

class Data {

public:

    enum Code {
        CHAR = 0,
        WCHAR,
        SHORT,
        INT,
        LONG,
        SIGNED_CHAR,
        UNSIGNED_CHAR,
        UNSIGNED_SHORT,
        UNSIGNED,
        UNSIGNED_LONG,
        FLOAT,
        DOUBLE,
        LONG_DOUBLE,
        //        BOOL,
        COMPLEX,
        DOUBLE_COMPLEX,
        //        LONG_DOUBLE_COMPLEX,
        BYTE,
        PACKED,
        SHORT_INT,
        INT_INT,
        LONG_INT,
        FLOAT_INT,
        DOUBLE_INT,
        LONG_DOUBLE_INT,
        LONG_LONG,
        TWO_LONG,
        TWO_LONG_LONG,
        MAX_DATA_CODE
    };

    template <class T>
    struct Type {
        static const char* name();
        static Code code();
        static size_t size();
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif

/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_DataType_h
#define eckit_mpi_DataType_h

#include <cstddef>
#include <vector>

#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

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
        MAX_DATA_CODE
    };

    template <class T>
    struct Type {
        static const char* name();
        static Code code();
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif

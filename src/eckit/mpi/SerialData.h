/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef eckit_mpi_SerialData_h
#define eckit_mpi_SerialData_h

#include <complex>
#include "eckit/mpi/DataType.h"

namespace eckit::mpi {

static size_t dataSize[Data::MAX_DATA_CODE] = {
    /*[Data::CHAR]                 = */ sizeof(char),
    /*[Data::WCHAR]                = */ sizeof(wchar_t),
    /*[Data::SHORT]                = */ sizeof(short),
    /*[Data::INT]                  = */ sizeof(int),
    /*[Data::LONG]                 = */ sizeof(long),
    /*[Data::SIGNED_CHAR]          = */ sizeof(signed char),
    /*[Data::UNSIGNED_CHAR]        = */ sizeof(unsigned char),
    /*[Data::UNSIGNED_SHORT]       = */ sizeof(unsigned short),
    /*[Data::UNSIGNED]             = */ sizeof(unsigned int),
    /*[Data::UNSIGNED_LONG]        = */ sizeof(unsigned long),
    /*[Data::FLOAT]                = */ sizeof(float),
    /*[Data::DOUBLE]               = */ sizeof(double),
    /*[Data::LONG_DOUBLE]          = */ sizeof(long double),
    //    /*[Data::BOOL]                 = */ sizeof(bool),
    /*[Data::COMPLEX]              = */ sizeof(std::complex<float>),
    /*[Data::DOUBLE_COMPLEX]       = */ sizeof(std::complex<double>),
    //    /*[Data::LONG_DOUBLE_COMPLEX]  = */ sizeof(std::complex<long double>),
    /*[Data::BYTE]                 = */ sizeof(char),
    /*[Data::PACKED]               = */ sizeof(char),
    /*[Data::SHORT_INT]            = */ sizeof(std::pair<short, int>),
    /*[Data::INT_INT]              = */ sizeof(std::pair<int, int>),
    /*[Data::LONG_INT]             = */ sizeof(std::pair<long, int>),
    /*[Data::FLOAT_INT]            = */ sizeof(std::pair<float, int>),
    /*[Data::DOUBLE_INT]           = */ sizeof(std::pair<double, int>),
    /*[Data::LONG_DOUBLE_INT]      = */ sizeof(std::pair<long double, int>),
    /*[Data::LONG_LONG]            = */ sizeof(long long),
    /*[Data::TWO_LONG]             = */ 2 * sizeof(long),
    /*[Data::TWO_LONG_LONG]        = */ 2 * sizeof(long long),
};

}  // namespace eckit::mpi

#endif

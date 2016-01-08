/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "grib_api.h"

// Some grib_api privates....

extern "C"
{
    double grib_long_to_ibm(unsigned long);
    double grib_long_to_ieee(unsigned long);
    long grib_decode_signed_long(const unsigned char* p, long offset, int bits);
    unsigned long grib_decode_unsigned_long(const unsigned char* p, long* offset, int bits);
    double grib_power(long s, long n);
    double grib_nearest_distance(double radius,double lon1, double lat1, double lon2, double lat2);
    int grib_read_any_headers_only_from_file(grib_context* ctx,FILE* f,void* buffer,size_t* len);
}

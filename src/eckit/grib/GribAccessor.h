/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef GribAccerssor_H
#define GribAccerssor_H

#include "eckit/grib/Grib.h"
#include "eckit/log/Log.h"

#define  GRIB 0x47524942

//-----------------------------------------------------------------------------

//extern "C" {
//    double grib_long_to_ibm(unsigned long);
//    double grib_long_to_ieee(unsigned long);
//    long grib_decode_signed_long(const unsigned char* p, long offset, int bits);
//    unsigned long grib_decode_unsigned_long(const unsigned char* p, long* offset, int bits);
//    double grib_power(long s, long n);
//    double grib_nearest_distance(double radius,double lon1, double lat1, double lon2, double lat2);
//    int grib_read_any_headers_only_from_file(grib_context* ctx,FILE* f,void* buffer,size_t* len);
//}


inline void decode(const unsigned  char* buf, int size, double& x) {
    long bitp = 0;
    x = grib_long_to_ibm(grib_decode_unsigned_long(buf, &bitp, 8*size));
}

inline void decode(const unsigned char* buf, int size, unsigned long& x) {
    long bitp = 0;
    x = grib_decode_unsigned_long(buf, &bitp, 8*size);
}

inline void decode(const unsigned char* buf, int size, long& x) {
    long bitp = 0;
    x = grib_decode_signed_long(buf, bitp, size);
}

inline void get_value(grib_handle* h, const std::string& name, double& x, bool quiet = false) {
    x = 0;
    int err = grib_get_double(h, name.c_str(), &x);
    if(err && !quiet) eckit::Log::error() << name << ": " << grib_get_error_message(err) << std::endl;
}


inline void get_value(grib_handle* h, const std::string& name,  unsigned long& x, bool quiet = false) {
    long y = 0;
    int err = grib_get_long(h, name.c_str(), &y);
    if(err && !quiet) eckit::Log::error() << name << ": " << grib_get_error_message(err) << std::endl;
    x = y;
}

inline void get_value(grib_handle* h, const std::string& name,  long& x, bool quiet = false) {
    x = 0;
    int err = grib_get_long(h, name.c_str(), &x);
    if(err && !quiet) eckit::Log::error() << name << ": " << grib_get_error_message(err) << std::endl;
}

inline void get_value(grib_handle* h, const std::string& name,  std::string& x, bool quiet = false) {
    char buf[1024];
    size_t s = sizeof(buf);
    buf[0] = 0;
    int err = grib_get_string(h, name.c_str(), buf, &s);
    if(err && !quiet) eckit::Log::error() << name << ": " << grib_get_error_message(err) << std::endl;
    x = buf;
}

template<class T>
class GribAccessor {
    std::string name_;

    public:
    GribAccessor(const std::string& name): name_(name) {}

    /* get_value(h, name_, value_); */

    /* grib_get_offset(h,name_.c_str(),&offset_); */

    /*
       unsigned char bytes[4];
       length_ = sizeof(bytes);
       grib_get_bytes(h, name_.c_str(), bytes, &length_);
       */
    //cout << "GribAccessor " << name_ << ", offset=" << offset_ << ", length=" << length_ << endl;

    /*
       T value(FILE* f) {
       unsigned char buf[4];

       fseek(f,offset_,SEEK_SET);
       fread(buf,length_,1,f);

       T x;
       decode(buf, length_, x);
    //cout << "value " << name_ << " => " << x << " " << offset_ << endl;
    return x;
    }
    */

    /* size_t offset(grib_handle *h) const { return offset_; } */

    T value(grib_handle *h) const { 
        T value;
        get_value(h, name_, value);
        return value; 
    }

    T value(grib_handle *h,T def) const { 
        T value = def;
        get_value(h, name_, value, true);
        return value; 
    }


    T operator()(grib_handle *h) const
    {
        return value(h);
    }

    T operator()(grib_handle *h, T def) const
    {
        return value(h, def);
    }

};

//-----------------------------------------------------------------------------

#endif

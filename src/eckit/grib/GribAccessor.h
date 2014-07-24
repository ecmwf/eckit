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
#include "eckit/exception/Exceptions.h"

//#define  GRIB 0x47524942

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

inline void check_error_code( const std::string& name, int err, bool quiet = false )
{
	if(err && !quiet) eckit::Log::error() << "GribAccessor(" << name << "): " << grib_get_error_message(err) << std::endl;
}

inline void decode(const unsigned  char* buf, int size, double& x)
{
	long bitp = 0;
	x = grib_long_to_ibm(grib_decode_unsigned_long(buf, &bitp, 8*size));
}

inline void decode(const unsigned char* buf, int size, unsigned long& x)
{
	long bitp = 0;
	x = grib_decode_unsigned_long(buf, &bitp, 8*size);
}

inline void decode(const unsigned char* buf, int size, long& x) {
	long bitp = 0;
	x = grib_decode_signed_long(buf, bitp, size);
}

inline void get_value(grib_handle* h, const std::string& name, double& x, bool quiet = false)
{
	x = 0;
	int err = grib_get_double(h, name.c_str(), &x);
	check_error_code(name,err,quiet);
}


inline void get_value(grib_handle* h, const std::string& name,  unsigned long& x, bool quiet = false)
{
	long y = 0;
	int err = grib_get_long(h, name.c_str(), &y);
	check_error_code(name,err,quiet);
	x = y;
}

inline void get_value(grib_handle* h, const std::string& name,  long& x, bool quiet = false)
{
	x = 0;
	int err = grib_get_long(h, name.c_str(), &x);
	check_error_code(name,err,quiet);
}

inline void get_value(grib_handle* h, const std::string& name,  std::string& x, bool quiet = false)
{
	char buf[1024];
	size_t s = sizeof(buf);
	buf[0] = 0;
	int err = grib_get_string(h, name.c_str(), buf, &s);
	check_error_code(name,err,quiet);
	x = buf;
}

inline void get_value(grib_handle* h, const std::string& name,  std::vector<long>& x, bool quiet = false)
{
	int err = 0;
	size_t sz = 0;
	err = grib_get_size(h,name.c_str(),&sz); check_error_code(name,err,quiet);
	x.resize(sz);
	err = grib_get_long_array(h,name.c_str(),&x[0],&sz); check_error_code(name,err,quiet);
	ASSERT( x.size() == sz );
}

inline void get_value(grib_handle* h, const std::string& name,  std::vector<double>& x, bool quiet = false)
{
	int err = 0;
	size_t sz = 0;
	err = grib_get_size(h,name.c_str(),&sz); check_error_code(name,err,quiet);
	x.resize(sz);
	err = grib_get_double_array(h,name.c_str(),&x[0],&sz); check_error_code(name,err,quiet);
	ASSERT( x.size() == sz );
}

//------------------------------------------------------------------------------------------------------

template<class T>
class GribAccessor {

private: // members

	std::string name_;

public: // methods

	GribAccessor(const std::string& name): name_(name) {}

	T value(grib_handle *h) const
	{
		T value;
		get_value(h, name_, value);
		return value;
	}

	T value(grib_handle *h,T def) const
	{
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

//------------------------------------------------------------------------------------------------------

std::string grib_geography_hash( grib_handle* h );

std::string grib_geography_hash( const std::string& fname );

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

#endif

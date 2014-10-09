/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "grib_api.h"

#include "eckit/grib/GribMutator.h"
#include "eckit/grib/GribHandle.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace grib {

static void check_error_code( const std::string& name, int err, bool quiet = false )
{
	if(err && !quiet)
		eckit::Log::error() << "grib_set_value(" << name << "): " << grib_get_error_message(err) << std::endl;
}

void grib_set_value(GribHandle& h, const std::string& name, double x, bool quiet)
{
	int err = grib_set_double(h.raw(), name.c_str(), x);
	check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, unsigned long x, bool quiet)
{
	int err = grib_set_long(h.raw(), name.c_str(), x);
	check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, long x, bool quiet)
{
	int err = grib_set_long(h.raw(), name.c_str(), x);
	check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, bool xy, bool quiet)
{
   long x = 0;
   if (xy) x = 1;
   int err = grib_set_long(h.raw(), name.c_str(), x);
   check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, std::string x, bool quiet)
{
	size_t s = x.size();
	int err = grib_set_string(h.raw(), name.c_str(), x.c_str(), &s );
	check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, std::vector<long> x, bool quiet)
{
	int err = grib_set_long_array(h.raw(),name.c_str(),&x[0],x.size());
	check_error_code(name,err,quiet);
}

void grib_set_value(GribHandle& h, const std::string& name, std::vector<double> x, bool quiet)
{
	int err = grib_set_double_array(h.raw(),name.c_str(),&x[0],x.size());
	check_error_code(name,err,quiet);
}

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

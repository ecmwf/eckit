/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_grib_GribAccessor_H
#define eckit_grib_GribAccessor_H

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/grib/GribHandle.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

void grib_get_value(const GribHandle& h, const std::string& name, double& x, bool quiet = false);

void grib_get_value(const GribHandle& h, const std::string& name,  unsigned long& x, bool quiet = false);

void grib_get_value(const GribHandle& h, const std::string& name,  long& x, bool quiet = false);

void grib_get_value(const GribHandle& h, const std::string& name,  std::string& x, bool quiet = false);

void grib_get_value(const GribHandle& h, const std::string& name,  std::vector<long>& x, bool quiet = false);

void grib_get_value(const GribHandle& h, const std::string& name,  std::vector<double>& x, bool quiet = false);

//------------------------------------------------------------------------------------------------------

template<class T>
class GribAccessor {

private: // members

	std::string name_;

public: // methods

	GribAccessor(const std::string& name): name_(name) {}

	T value(const GribHandle& h) const
	{
		T value;
		grib_get_value(h, name_, value);
		return value;
	}

	T value(const GribHandle& h,T def) const
	{
		T value = def;
		grib_get_value(h, name_, value, true);
		return value;
	}

	T operator()(const GribHandle& h) const
	{
		return value(h);
	}

	T operator()(const GribHandle& h, T def) const
	{
		return value(h, def);
	}

};

//------------------------------------------------------------------------------------------------------

std::string grib_geography_hash( const GribHandle& h );

std::string grib_geography_hash( const std::string& fname );

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

#endif

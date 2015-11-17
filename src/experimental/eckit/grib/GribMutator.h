/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_grib_GribMutator_H
#define eckit_grib_GribMutator_H

#include "eckit/exception/Exceptions.h"
#include "eckit/grib/GribHandle.h"

struct grib_handle;

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

void grib_set_value(GribHandle& h, const std::string& name, double x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, unsigned long x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, long x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, bool x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, std::string x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, std::vector<long> x, bool quiet = false);

void grib_set_value(GribHandle& h, const std::string& name, std::vector<double> x, bool quiet = false);

//------------------------------------------------------------------------------------------------------

template<class T>
class GribMutator {

private: // members

	std::string name_;

public: // methods

	GribMutator(const std::string& name): name_(name) {}

	void set(GribHandle& h, T value) const
	{
		grib_set_value(h, name_, value);
	}

	void operator() (GribHandle& h, T value) const
	{
		grib_set_value(h, name_, value);
	}

};

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

#endif

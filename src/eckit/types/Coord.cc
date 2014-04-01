/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Coord.h"
#include "eckit/persist/DumpLoad.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Longitude::Longitude(double l):
	Coord(l)
{
	while (value_ <    0.0) value_ += 360.0;
	while (value_ >= 360.0) value_ -= 360.0;
}

Latitude::Latitude(double l):
	Coord(l)
{
	ASSERT(l <= 90.0 && l >= -90.0);
}

void Coord::dump(DumpLoad& a) const
{
	a.dump(value_);
}

void Coord::load(DumpLoad& a)
{
	a.load(value_);
}

//-----------------------------------------------------------------------------

} // namespace eckit


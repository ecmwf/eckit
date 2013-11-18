/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/grib/GribFieldMemoryStrategy.h"

namespace eckit {

//--------------------------------------------------------------------------------------------

GribFieldMemoryStrategy::GribFieldMemoryStrategy()
{
}

GribFieldMemoryStrategy::~GribFieldMemoryStrategy()
{
}

void eckit::GribFieldMemoryStrategy::touch(eckit::GribField &)
{
}


void eckit::GribFieldMemoryStrategy::release(eckit::GribField &)
{
}


void eckit::GribFieldMemoryStrategy::remove(eckit::GribField &)
{
}


void eckit::GribFieldMemoryStrategy::newValues(eckit::GribField &)
{
}


void eckit::GribFieldMemoryStrategy::newHandle(eckit::GribField &)
{
}

//--------------------------------------------------------------------------------------------

} // namespace eckit

/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/ColorizeFormat.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static std::ostream& noop(std::ostream& s) { return s; }

ColorizeFormat::ColorizeFormat(std::size_t size)
 : FormatBuffer(size),
   begin_(noop),
   end_(noop)
{
}

void ColorizeFormat::process(const char *begin, const char *end)
{
    target()->write(begin,end-begin);
}

//-----------------------------------------------------------------------------

} // namespace eckit

/*
 * (C) Copyright 1996-2016 ECMWF.
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
 : FormatBuffer(0,size),
   begin_(noop),
   end_(noop)
{
}

void ColorizeFormat::beginLine()
{
//    *target() << begin_ << prefix_;
}

void ColorizeFormat::endLine()
{
//    *target() << end_;
}

std::string ColorizeFormat::prefix() const
{
    return prefix_;
}

void ColorizeFormat::prefix(const std::string& p)
{
    prefix_ = p;
}

//-----------------------------------------------------------------------------

} // namespace eckit

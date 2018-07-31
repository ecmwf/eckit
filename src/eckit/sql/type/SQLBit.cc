/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/type/SQLBit.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {
namespace type {

SQLBit::SQLBit(const std::string& name, unsigned long mask, unsigned long shift):
	type::SQLType(name),
	mask_(mask),
	shift_(shift)
{}

SQLBit::~SQLBit() {}

size_t SQLBit::size() const
{
    NOTIMP;
    // This should not be calles
    return sizeof(long);
}

void SQLBit::output(SQLOutput& o, double x, bool missing) const
{
	//Log::info() << "SQLBit::output: x=" << x << ", missing=" << missing << std::endl;
	//s << ((m & mask_) >> shift_);
    // TODO: does it work like this? test!
    o.outputUnsignedInt(x, missing);
}

} // namespace type 
} // namespace sql 
} // namespace eckit 

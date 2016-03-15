/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/web/HtmlObject.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


HtmlObject::HtmlObject()
{
}

HtmlObject::~HtmlObject()
{
}

void HtmlObject::html(std::ostream& s,Url& url)
{
	print(s);
}

void HtmlObject::substitute(std::ostream& s,const std::string& p)
{
	s << '%' << p << '%';
}

void HtmlObject::print(std::ostream& s) const
{
	s << "No print method defined for this object";
}

void HtmlObject::java(JavaAgent&)
{
}

//-----------------------------------------------------------------------------

} // namespace eckit


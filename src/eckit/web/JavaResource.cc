/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/PathName.h"
#include "eckit/web/HttpBuf.h"
#include "eckit/web/JavaResource.h"
#include "eckit/web/Url.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


JavaResource::JavaResource():
	HtmlResource("/java")
{
}

JavaResource::~JavaResource()
{
}

void JavaResource::html(std::ostream& s,Url& url)
{
	url.dontCache();

	eckit::PathName path("~/http/" + url.name());
	char c;

	std::ifstream in(path.localPath());
	if(!in)	
	{
		(url.headerOut()).status(404);  // Not Found
		s << path << ": " << Log::syserr << std::endl;
	}
	else
	{
	//	(url.headerOut()).type("image/gif");

		s << HttpBuf::dontEncode;
		while(in.get(c))
			s << c;
		s << HttpBuf::doEncode;
	}
}

static JavaResource javaResourceInstance;

//-----------------------------------------------------------------------------

} // namespace eckit


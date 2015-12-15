/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdPipe.h"
#include "eckit/web/CgiResource.h"
#include "eckit/web/HttpBuf.h"
#include "eckit/web/Url.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

CgiResource::CgiResource():
	HtmlResource("/cgi")
{
}

CgiResource::~CgiResource()
{
}

void CgiResource::html(std::ostream& s,Url& url)
{
	eckit::PathName path("~/http/" + url.name());
    std::ostringstream cmd;

	std::string mode = url["parameter"];

	if(mode == "")
	{
		cmd << "env ";
		url.cgiParam(cmd,' ');
        cmd << " " << path;
	}
	else
	{
		cmd << path << ' ';
        url.cgiParam(cmd,' ');
	}

    StdPipe pipe(cmd.str(),"r");
	char line[1024];

	s << HttpBuf::dontEncode;

	while(fgets(line,sizeof(line),pipe))
		s << line;

	s << HttpBuf::doEncode;
}

static CgiResource cgiResourceInstance;

//-----------------------------------------------------------------------------

} // namespace eckit


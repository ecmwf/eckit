/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HtmlResource.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef HtmlResource_H
#define HtmlResource_H

#include "eckit/memory/NonCopyable.h"
#include "eckit/web/HtmlObject.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;

class HtmlResource : public HtmlObject,
                     public eckit::NonCopyable {
public:

// -- Contructors

	HtmlResource(const string&);

// -- Destructor

	virtual ~HtmlResource();


// -- Methods

	virtual bool restricted() { return false; }


// -- Class methods

    static void dispatch(eckit::Stream&,std::istream&,std::ostream&,Url&);
	static void index(std::ostream&,Url&);

private:

    typedef map<string,HtmlResource*,std::less<string> > Map;

// -- Class members

	static Map* resources_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HtmlObject.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef HtmlObject_H
#define HtmlObject_H

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Url;
class JavaAgent;
class Bless;

class HtmlObject {
public:

	HtmlObject();

#include "eckit/web/HtmlObject.b"

	virtual ~HtmlObject();

	virtual void html(std::ostream&,Url&);
	virtual void java(JavaAgent&);
	virtual void substitute(std::ostream&,const std::string&);

protected:
	
	 virtual void print(std::ostream&) const; 

private:

	friend std::ostream& operator<<(std::ostream& s,const HtmlObject& p)
		{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif

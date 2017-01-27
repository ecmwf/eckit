/*
 * (C) Copyright 1996-2017 ECMWF.
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
class HtmlResource;

class HtmlResourceMap {

private:
	typedef std::map<std::string,HtmlResource*,std::less<std::string> > Map;
	Map * ptr_;

public:
	void init() {
		if (!ptr_) ptr_ = new Map;
	}

	~HtmlResourceMap() {
		if (ptr_) {
			delete ptr_;
			ptr_ = 0;
		}
	}

	Map * operator->() const {
		return ptr_;
	}

	Map & operator*() const {
		return *ptr_;
	}

	typedef Map::iterator iterator;
};

class HtmlResource : public HtmlObject,
                     public eckit::NonCopyable {
public:

// -- Contructors

	HtmlResource(const std::string&);

// -- Destructor

	virtual ~HtmlResource();


// -- Methods

	virtual bool restricted() { return false; }


// -- Class methods

    static void dispatch(eckit::Stream&,std::istream&,std::ostream&,Url&);
	static void index(std::ostream&,Url&);

	static HtmlResourceMap resources_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

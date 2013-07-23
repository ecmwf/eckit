/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Url.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef Url_H
#define Url_H

#include "eckit/value/Value.h"
#include "eckit/web/HttpHeader.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Url;

//-----------------------------------------------------------------------------

class UrlAccess {
	Url&   url_;
	string s_;
	

public:
	UrlAccess(Url& url,const string& s) : url_(url), s_(s) {}

	operator string();
	operator long();

	UrlAccess& operator=(const string&);
	UrlAccess& operator=(long);
};

//-----------------------------------------------------------------------------

class Url : private eckit::NonCopyable {
public:

	enum { 
		notFound = 404
	};

// -- Contructors

	Url(istream&);
	Url(const string&);

// -- Destructor

    ~Url();

// -- Operators
	
	UrlAccess operator[](const string&);

// -- Methods

	void erase(const string&);


	void set(const string&,const string&);
	const string& get(const string&);

	const string& method() { return method_;}


	HttpHeader&   headerIn();
	HttpHeader&   headerOut();

	const HttpHeader&   headerIn() const;
	const HttpHeader&   headerOut() const;

	string str() const;
	string name() const;


	int size() const;
	const string& operator[](int) const;

	bool authenticated()
		{ return headerIn().authenticated(); }

	void authenticate(const string& realm = "MARS") 
		{ headerOut().authenticate(realm); }

	void status(int s)
		{ headerOut().status(s); }

	void forward(const string& s)
		{ headerOut().forward(s); }

	void dontCache()
		{ headerOut().dontCache(); }

	void cgiParam(ostream&,char sep = ' ') const;

    const eckit::Value& json() const
        { return json_; }

protected:

// -- Methods
	
    void print(ostream&) const;

private:

// -- Members

	typedef map<string,string,less<string> > Map;

	Map            map_;
	vector<string> url_;
	HttpHeader     in_;
	HttpHeader     out_;

	string		   method_;

    eckit::Value          json_;

// -- Methods

	void parse(const string&,bool);
	void parse(istream&);

// -- Friends

	friend ostream& operator<<(ostream& s,const Url& p)
		{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif

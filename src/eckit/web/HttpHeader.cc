/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/web/HttpHeader.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// Check the <A HREF=http://www.ics.uci.edu/pub/ietf/http/rfc1945.html">HTTP/1.0</A> syntax
// Check the <A HREF=http://src.doc.ic.ac.uk/computing/internet/rfc/rfc2068.txt">HTTP/1.1</A> syntax

const string WWW_Authenticate = "WWW-Authenticate";
const string Authorization    = "Authorization";
const string Content_Type     = "Content-Type";
const string Content_Length   = "Content-Length";
const string Location         = "Location";
const string DefaultType      = "application/x-www-form-urlencoded";

bool HttpHeader::compare::operator()(const string& a,const string& b) const
{
	return strcasecmp(a.c_str(),b.c_str()) < 0;
}


HttpHeader::HttpHeader():
	version_("HTTP/1.0"),
	statusCode_(200),
	content_(0,false)
{
	header_[Content_Type] = " text/html";
}

HttpHeader& HttpHeader::operator=(map<string,string,less<string> >& parsed)
{
	for(map<string,string,less<string> >::const_iterator i = parsed.begin(); 
		i != parsed.end(); ++i) 
			header_[(*i).first] = (*i).second;

	Map::const_iterator j = header_.find(Content_Length);

	if(j != header_.end())
		contentLength_ = atol(((*j).second).c_str());
	else
		contentLength_ = 0;

	return *this; 
}

HttpHeader::~HttpHeader()
{
}

void HttpHeader::print(ostream& s) const
{

	// Status line + CRLF
	s << version_ << ' ' << statusCode_ << " Document follows" << '\r' << '\n';

	// General-Header : Date, Pragma

	s << "Cache-Control: no-cache" << endl;

	// Response-Header: Location, Server, WWW-Authenticate
	s << "MIME-Version: 1.0" << endl;

	Map::const_iterator i = header_.find(Location);
	if( i != header_.end() )
		s << (*i).first <<  ": " << (*i).second << endl;

	i = header_.find(WWW_Authenticate);
	if( i != header_.end() )
	{
		s << "This is the debug output... " << endl;
		s << (*i).first <<  ": " << (*i).second << endl;
	}

	// Entity-Header  : Allow, Content-Encoding, 
	// Content-Length, Content-Type, Expires, Last-Modified

	s << Content_Length << ": " << contentLength_ + content_.size() << endl;

	i = header_.find(Content_Type);

	if( i != header_.end() )
		s << (*i).first <<  ": " << (*i).second;

	// CRLF
	s << '\r' << '\n' << endl;


	long len = content_.size();
	const char *p  = content_;
	while(len-->0)
		s.put(*p++);
}

void HttpHeader::forward(const string& s)
{
	header_[Location] = s;
}

void HttpHeader::length(const long l)
{
	contentLength_ = l;	
}

long HttpHeader::contentLength() const
{
	return contentLength_;
}

void HttpHeader::type(const string& s)
{
	header_[Content_Type] = s;
}

const string& HttpHeader::type() const
{
    Map::const_iterator i = header_.find(Content_Type);

    if( i != header_.end() )
        return (*i).second;

    return DefaultType;
}

void HttpHeader::status(const long code)
{
	statusCode_ = code;
}

void HttpHeader::authenticate(const string& login)
{
	header_[WWW_Authenticate] = ("Basic realm=\"" + login + "\""); 
	status(401);
}

void HttpHeader::dontCache()
{
}

bool HttpHeader::authenticated() const
{
	Map::const_iterator i = header_.find(Authorization);

	if(i != header_.end())
	{
		const char *s = (*i).second.c_str();

		while(*s != ' ' && *s != '\t') s++;
		while(*s == ' ' || *s == '\t') s++;

		unsigned char b64[256];
		for(int j = 0; j < 256; j++) b64[j] = 64;

		for(char c = 'A'; c <= 'Z' ; c++) b64[c] = c - 'A';
		for(char c = 'a'; c <= 'z' ; c++) b64[c] = c - 'a' + 26;
		for(char c = '0'; c <= '9' ; c++) b64[c] = c - '0' + 52;

		b64['+']=62; 
		b64['/']=63;

		const unsigned char *p = (const unsigned char*)s;

		string q;

		int n = 2;

		while(b64[*p] < 64 && b64[*(p+1)] < 64)
		{
			q += char((b64[p[0]] << n) | (b64[p[1]] >> (6-n)));
			n += 2;
			if(n == 8) {
				p++;
				n = 2;
			}
			p++;
		}

		cout << q << endl;


		Tokenizer parse(":");
		vector<string> v;
		parse(q,v);
		if(v.size()==2 && v[0] == "mars" && v[1]=="clave")
		{
			Log::info() << "client authenticated " << q << " -> " << (*i).second << endl ;
			return true;
		}

		Log::info() << "client denied " << q << " -> " << (*i).second << endl ;
	}

	return false;
}

void HttpHeader::content(const char* p,long len)
{
	content_.resize(len);
	::memcpy((char*)content_,p,len);
}

void HttpHeader::setHeader(const string& k,const string& v)
{
	header_[k] = v;
}

const string& HttpHeader::getHeader(const string& k) const
{
	return ((HttpHeader*)this)->header_[k];
}

//-----------------------------------------------------------------------------

} // namespace eckit


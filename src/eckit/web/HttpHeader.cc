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

const std::string WWW_Authenticate = "WWW-Authenticate";
const std::string Authorization    = "Authorization";
const std::string Content_Type     = "Content-Type";
const std::string Content_Length   = "Content-Length";
const std::string Location         = "Location";
const std::string DefaultType      = "application/x-www-form-urlencoded";

bool HttpHeader::compare::operator()(const std::string& a,const std::string& b) const
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

HttpHeader& HttpHeader::operator=(std::map<std::string,std::string,std::less<std::string> >& parsed)
{
    for(std::map<std::string,std::string,std::less<std::string> >::const_iterator i = parsed.begin();
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

void HttpHeader::print(std::ostream& s) const
{

	// Status line + CRLF
	s << version_ << ' ' << statusCode_ << " Document follows" << '\r' << '\n';

	// General-Header : Date, Pragma

	s << "Cache-Control: no-cache" << std::endl;

	// Response-Header: Location, Server, WWW-Authenticate
	s << "MIME-Version: 1.0" << std::endl;

	Map::const_iterator i = header_.find(Location);
	if( i != header_.end() )
		s << (*i).first <<  ": " << (*i).second << std::endl;

	i = header_.find(WWW_Authenticate);
	if( i != header_.end() )
	{
		s << "This is the debug output... " << std::endl;
		s << (*i).first <<  ": " << (*i).second << std::endl;
	}

	// Entity-Header  : Allow, Content-Encoding, 
	// Content-Length, Content-Type, Expires, Last-Modified

	s << Content_Length << ": " << contentLength_ + content_.size() << std::endl;

	i = header_.find(Content_Type);

	if( i != header_.end() )
		s << (*i).first <<  ": " << (*i).second;

	// CRLF
	s << '\r' << '\n' << std::endl;


	long len = content_.size();
	const char *p  = content_;
	while(len-->0)
		s.put(*p++);
}

void HttpHeader::forward(const std::string& s)
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

void HttpHeader::type(const std::string& s)
{
	header_[Content_Type] = s;
}

const std::string& HttpHeader::type() const
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

void HttpHeader::authenticate(const std::string& login)
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

		std::string q;

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

		std::cout << q << std::endl;


		Tokenizer parse(":");
		std::vector<std::string> v;
		parse(q,v);
		if(v.size()==2 && v[0] == "mars" && v[1]=="clave")
		{
            Log::info() << "client authenticated " << q << " -> " << (*i).second << std::endl;
			return true;
		}

        Log::info() << "client denied " << q << " -> " << (*i).second << std::endl;
	}

	return false;
}

void HttpHeader::content(const char* p,long len)
{
	content_.resize(len);
	::memcpy((char*)content_,p,len);
}

void HttpHeader::setHeader(const std::string& k,const std::string& v)
{
	header_[k] = v;
}

const std::string& HttpHeader::getHeader(const std::string& k) const
{
	return ((HttpHeader*)this)->header_[k];
}

//-----------------------------------------------------------------------------

} // namespace eckit


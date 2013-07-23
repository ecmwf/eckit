/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/log/Log.h"
#include "eckit/compat/StrStream.h"
#include "eckit/parser/Translator.h"
#include "eckit/parser/JSONParser.h"
#include "eckit/web/Html.h"
#include "eckit/web/Url.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


inline void header(char c)
{
#if 0
	if(isprint(c))
		cout << "header: " << c << endl;
	else
		cout << "header: " << hex << int(c) << dec << endl;
#endif
}

void Url::parse(const string& url, bool param)
{

	int size    = url.length();
	int index   = 0;
	string s,p;

	while(index < size)
    {

		switch(url[index])
		{
			case '?':
				param = true;
				if(s != "") url_.push_back(Html::removeHex(s));
				s     = "";
				break;

			case '=':
				p     = s;
				s     = "";
				break;

			case '&':
				map_[Html::removeHex(p)] = Html::removeHex(s);
				s       = "";
				p       = "";
				break;

			case '+':
				// Do we need a multimap?
				s += url[index];
				break;

			case '/':
				if( ! param )
				{
					if(s != "") url_.push_back(Html::removeHex(s));
					s = "";
				}
				else s += url[index];
				break;

			default:
				s += url[index];
				break;
		}

		++index;
    }

	if( ! param )
	{
		if(s != "") url_.push_back(Html::removeHex(s));
	}
	else {
		if(p != "")
			map_[Html::removeHex(p)] = Html::removeHex(s);
	}

}

Url::Url(istream& in)
{
	string url;

	in >> method_;
	in >> url;
	parse(url,false);

	char c = 0;
	while(in.get(c) && c != '\n')  
		header(c);

	parse(in);
	Log::debug() << "Incomming url-> " << *this << endl;
}

void Url::parse(istream& in)
{

	char c = 0;

	map<string,string,less<string> > m;

	for(;;)
	{
		string s;
		while(in.get(c) && c != ':' && c != '\r')
		{
			header(c);
			s += c;
		}

		if(c != ':')
		{
			header(c);
			in.get(c);    // Skip '\n'
			header(c);
			break;
		}

		header(c);
		while(in.get(c) && c == ' ' && c != '\r')
			header(c);
		
		if(c != '\r')
		{
			header(c);

			string r;
			r += c;
			while(in.get(c) && c != '\r')
			{
				header(c);
				r += c;
			}

			m[s] = r;
			in.get(c);    // Skip '\n'
			header(c);
		}
	}

	in_ = m;

	long len   = in_.contentLength();

	if(len)
	{
        static const string FormType      = "application/x-www-form-urlencoded";
        static const string JSONType      = "application/json";

        //bool ascii = true;
		Buffer content(len);
        const string& type = in_.type();


		char *p = content;

		for(int i = 0; i < len; i++)
		{
			in.get(c); 
			header(c);
			*p++ = c;
		}

        if(type == FormType)
		{
			string s(static_cast<char*>(content),p - static_cast<char*>(content));
			parse(s,true);
		}

        in_.content(content,len);

        if(type == JSONType)
        {
            const char* p = in_.content();
            istringstream in(p ? p : "null");

            JSONParser q(in);
            json_ = q.parse();
        }

	}

	Log::debug() << *this << endl;
}

Url::Url(const string& url):
	method_("GET")
{
	parse(url,false);
	Log::debug() << "Incomming url-> " << *this << endl;
}

Url::~Url()
{
}

string Url::name() const
{
	string s = "";
	for(vector<string>::const_iterator j = url_.begin(); j != url_.end(); ++j)
	{
		s += "/" ;
		s += *j ;
	}
	return s;
}

void Url::print(ostream& s) const
{
	for(vector<string>::const_iterator j = url_.begin();
		j != url_.end(); ++j)
		s << "/" << *j ;

	char c = '?';
	Map::const_iterator i;
	for(i = map_.begin(); i != map_.end(); ++i)
	{
		s << c << (*i).first  << '=' << (*i).second;
		c = '&';
	}

}

void Url::cgiParam(ostream& s,char sep) const
{
	char c = ' ';
	Map::const_iterator i;
	for(i = map_.begin(); i != map_.end(); ++i)
	{
		s << c << (*i).first  << '=' << (*i).second;
		c = sep;
	}

}

UrlAccess Url::operator[](const string& s) 
{
	return UrlAccess(*this,s);
}

void Url::set(const string& p,const string& s)
{
	map_[p] = s;
}

const string& Url::get(const string& s)
{
	return map_[s];
}

void Url::erase(const string& s)
{
	map_.erase(s);
}

const string& Url::operator[](int n) const
{
	return url_[n];
}


string Url::str() const
{
	StrStream s;
	s << *this << StrStream::ends;
	return string(s);
}

int Url::size() const
{
	return url_.size();
}

HttpHeader& Url::headerIn()
{
	return in_;
}

HttpHeader& Url::headerOut()
{
	return out_;
}

UrlAccess::operator long()
{
	return Translator<string,long>()(url_.get(s_));
}

UrlAccess::operator string()
{
	return url_.get(s_);
}

UrlAccess& UrlAccess::operator=(long n)
{
	url_.set(s_,Translator<long,string>()(n));
	return *this;
}

UrlAccess& UrlAccess::operator=(const string& s)
{
	url_.set(s_,s);
	return *this;
}

//-----------------------------------------------------------------------------

} // namespace eckit

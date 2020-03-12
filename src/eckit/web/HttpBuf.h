/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult

#ifndef eckit_web_HttpBuf_H
#define eckit_web_HttpBuf_H

#include "eckit/web/Url.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class HttpStream;

class HttpBuf : public std::streambuf {


	char    out_[4096];
	virtual int overflow(int c);
	virtual int sync();

	HttpStream& owner_;

public:

	HttpBuf(HttpStream&);
    ~HttpBuf();

    void reset();
	void write(std::ostream&, Url&);

	static std::ostream& dontEncode(std::ostream&);
	static std::ostream& doEncode(std::ostream&);

    void print(std::ostream&) const;

private:

	std::vector<char>  buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

class HttpStream : public std::ostream {
public:

    HttpStream();
	~HttpStream();

    void reset();
    void write(std::ostream&, Url&, DataHandle&);

    void print(std::ostream& s) const;

private:

    HttpBuf* buf_;

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

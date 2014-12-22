/*
 * (C) Copyright 1996-2014 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD5_H
#define eckit_utils_MD5_H

#include <stdlib.h>
#include <stdint.h>

#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class MD5 : private eckit::NonCopyable {

public:

	MD5();

	MD5( const std::string& );

	MD5( const void* data, size_t len );

	~MD5();

	void add(const void*, long);
	
    operator std::string();

	std::string digest();

private:

	struct State
	{
		uint64_t size;

		unsigned long words[64];
		unsigned long word_count;

		unsigned char bytes[4];
		unsigned long byte_count;

		unsigned long h0;
		unsigned long h1;
		unsigned long h2;
		unsigned long h3;
	};

protected:

	void md5_init(State* s);
	void md5_add(State* s,const void* data,size_t len);
	void md5_end(State* s, char *digest);
	void md5_flush(State* s);

private:

	State s_;

	std::string digest_; ///< cached digest

};

} // end namespace eckit

#endif

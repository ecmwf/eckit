/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/URLHandle.h
// Baudouin Raoult - ECMWF Jun 17

#ifndef eckit_filesystem_URLHandle_h
#define eckit_filesystem_URLHandle_h

#include "eckit/io/DataHandle.h"
#include <curl/curl.h>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class URLHandle : public DataHandle {
public:

// -- Exceptions

// -- Contructors

	URLHandle(const std::string&);
	URLHandle(Stream&);

// -- Destructor

	~URLHandle() {}

// -- Overridden methods

	// From DataHandle

	virtual Length openForRead();
	virtual void openForWrite(const Length&);
	virtual void openForAppend(const Length&);

	virtual long read(void*, long);
	virtual long write(const void*, long);
	virtual void close();
	// virtual void rewind();
	virtual void print(std::ostream&) const;

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	std::string url_;
	std::string method_;


	int active_;
	long long pos_;
	size_t errors_;
	std::string lastError_;

	CURLcode curl_error_;
	CURLM *multi_handle_;
	CURL *curl_;
	struct curl_slist *chunk_;


	size_t len_;
	char* buffer_;

	std::map<std::string, std::string> receivedHeaders_;


// -- Methods

	void waitForData(size_t size);

	void call(const char* what, CURLcode code);
	void call(const char* what, CURLMcode code);

	size_t transferRead(void *ptr, size_t size);
	size_t transferWrite( void *ptr, size_t size, size_t nmemb);
	size_t receiveHeaders( void *ptr, size_t size, size_t nmemb);

	static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
	static size_t headersCallback( void *ptr, size_t size, size_t nmemb, void *userdata);

	long long transferStart();

	int transferEnd();

// -- Class members

	static  ClassSpec               classSpec_;
	static  Reanimator<URLHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif

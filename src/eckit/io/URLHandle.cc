/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>

#include "eckit/io/URLHandle.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/utils/Translator.h"
#include "eckit/parser/Tokenizer.h"

#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#endif

//-----------------------------------------------------------------------------

namespace eckit {

#define _(a) call(#a, a)


//-----------------------------------------------------------------------------

ClassSpec URLHandle::classSpec_ = {&DataHandle::classSpec(), "URLHandle",};
Reanimator<URLHandle> URLHandle::reanimator_;


// curl_global_init(CURL_GLOBAL_DEFAULT);

void URLHandle::print(std::ostream&s) const
{
	s << "URLHandle[url=" << url_ << ']';
}

void URLHandle::encode(Stream&s) const
{
	NOTIMP;
	DataHandle::encode(s);
	s << url_;
	// s << headers_;
}

URLHandle::URLHandle(Stream&s):
	DataHandle(s)
{
	NOTIMP;
	s >> url_;
	// s >> headers_;
}

#ifdef ECKIT_HAVE_CURL


URLHandle::URLHandle(const std::string&url,
                     const Headers&headers,
                     const std::string&method):
	url_(url),
	sendHeaders_(headers),
	method_(method),
	active_(0),
	multi_(0),
	curl_(0),
	chunk_(0),
	body_(false),
	contentLength_(0)
{
}

URLHandle::~URLHandle() {
	cleanup();


}

void URLHandle::cleanup() {
	if (curl_) {
		if (multi_) {
			curl_multi_remove_handle(multi_, curl_);
		}
		curl_easy_cleanup(curl_);
	}

	if (chunk_) {
		curl_slist_free_all(chunk_);
	}

	if (multi_) {
		curl_multi_cleanup(multi_);
	}


	multi_ = 0;
	chunk_ = 0;
	curl_ = 0;

	buffer_.clear();
}


Length URLHandle::openForRead()
{
	curl_  = curl_easy_init();
	ASSERT(curl_);


	const char* verbose = "1";

	_(curl_easy_setopt(curl_, CURLOPT_VERBOSE, verbose));

	_(curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str()));
	_(curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L));


	chunk_ = curl_slist_append(chunk_, "Accept: application/json");
	chunk_ = curl_slist_append(chunk_, "Content-Type: application/json");
	chunk_ = curl_slist_append(chunk_, "charsets: utf-8");
	// chunk_ = curl_slist_append(chunk_, api->key);
	// chunk_ = curl_slist_append(chunk_, api->email);

#ifdef SKIP_PEER_VERIFICATION
	_(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L));
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
	_(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L));
#endif

	_(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, chunk_));

	_(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, method_.c_str()));
	_(curl_easy_setopt(curl_, CURLOPT_USERAGENT, "eckit/1.0"));

	transferStart();
	while (!body_) {
		waitForData(0);
	}

	for (Headers::const_iterator j = receivedHeaders_.begin(); j != receivedHeaders_.end(); ++j) {
		std::cout << (*j).first << "=[" << (*j).second << "]" << std::endl;
	}

	Headers::const_iterator j = receivedHeaders_.find("content-length");
	if (j == receivedHeaders_.end()) {
		std::ostringstream oss;
		oss << "URLHandle::openForRead(" << url_ << ") does not provide a content-length";
		throw eckit::SeriousBug(oss.str());
	}

	Translator<std::string, size_t> s2l;
	contentLength_ = s2l((*j).second);

	return contentLength_;

}

void URLHandle::openForWrite(const Length&)
{
	NOTIMP;
}

void URLHandle::openForAppend(const Length&)
{
	NOTIMP;
}

long URLHandle::read(void* buffer, long length)
{
	waitForData(length);
	return buffer_.read(buffer, length);
}

long URLHandle::write(const void* buffer, long length)
{
	NOTIMP;
}

void URLHandle::close()
{
	int code;
	curl_easy_getinfo (curl_, CURLINFO_RESPONSE_CODE, &code);

	if (code != 200) {
		std::ostringstream oss;
		oss << "URLHandle::close(" << url_ << ") returns code " << code;
		throw eckit::SeriousBug(oss.str());
	}

	cleanup();

}

// void URLHandle::rewind()
// {
// 	NOTIMP;
// }

void URLHandle::call(const char* what, CURLcode code)
{
	// curl_error_ = code;
	if (code != CURLE_OK)
	{
		std::ostringstream oss;
		oss << what << " failed: " << curl_easy_strerror(code);
		// errors_++;
	}
}


void URLHandle::call(const char* what, CURLMcode code)
{
	// curl_error_ = code;
	if (code != CURLE_OK)
	{
		std::ostringstream oss;
		oss << what << " failed: " << curl_multi_strerror(code);
		// errors_++;
	}
}

void URLHandle::waitForData(size_t size)
{
	fd_set fdr, fdw, fdx;
	struct timeval timeout;

	while (active_  >= 0 && size <= buffer_.length()) {
		int maxfd = -1;
		long time = -1;

		FD_ZERO(&fdr);
		FD_ZERO(&fdw);
		FD_ZERO(&fdx);

		_(curl_multi_timeout(multi_, &time));
		if (time >= 0) {
			timeout.tv_sec = time / 1000;
			if (timeout.tv_sec > 1)
				timeout.tv_sec = 1;
			else
				timeout.tv_usec = (time % 1000) * 1000;
		}
		else
		{
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
		}

		_(curl_multi_fdset(multi_, &fdr, &fdw, &fdx, &maxfd));

		SYSCALL (::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout));

		_(curl_multi_perform(multi_, &active_));


	}

}

size_t URLHandle::transferRead(void *ptr, size_t size)
{
	NOTIMP;

	return size;
}

size_t URLHandle::writeCallback( void *ptr, size_t size, size_t nmemb, void *userdata) {
	return reinterpret_cast<URLHandle*>(userdata)->transferWrite(ptr, size, nmemb);
}


size_t URLHandle::transferWrite( void *ptr, size_t size, size_t nmemb)
{
	std::cout << "URLHandle::transferWrite size = " << size * nmemb << std::endl;
	return buffer_.write(ptr, size * nmemb);
}

size_t URLHandle::headersCallback( void *ptr, size_t size, size_t nmemb, void *userdata) {
	return reinterpret_cast<URLHandle*>(userdata)->receiveHeaders(ptr, size, nmemb);
}


size_t URLHandle::receiveHeaders( void *ptr, size_t size, size_t nmemb)
{

	char* p    = (char*)ptr;
	int  count = size * nmemb;

	ASSERT(!body_);
	ASSERT(count >= 2);
	ASSERT(p[count - 1] == '\n');
	ASSERT(p[count - 2] == '\r');

	std::string line(p, p + count - 2);
	std::cout << line << std::endl;

	if (line.empty()) {
		body_ = true;
	}
	else {
		std::vector<std::string> v;

		Tokenizer parse(":");

		parse(line, v);
		if (v.size()  == 2) {
			receivedHeaders_[StringTools::lower(v[0])] = StringTools::trim(v[1]);
		}

	}
	return size * nmemb;

}


long long URLHandle::transferStart()
{


	if (!multi_) {
		multi_ = curl_multi_init();
	}


	buffer_.clear();

	_(curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION , &headersCallback));
	_(curl_easy_setopt(curl_, CURLOPT_HEADERDATA , this));

	_(curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &writeCallback));
	_(curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this));

	_(curl_multi_add_handle(multi_, curl_));

	_(curl_multi_perform(multi_, &active_));



	// if((api->pos == 0) &&(!api->active)) {
	// 	curl_multi_remove_handle(multi_handle, curl_);
	// 	api->error++;
	// }

	// return json_get_integer(json_object_find(api->value,"size"));

	return 0;

}

int URLHandle::transferEnd()
{
	// curl_easy_getinfo (curl_, CURLINFO_RESPONSE_CODE, &code_);
	// cleanup(api);
	// if (api->code != 200)
	// 	marslog(LOG_EROR, "Transfer return code is %d", api->code);
	// return api->code == 200;
	return 0;
}
#else

URLHandle::URLHandle(const std::string&url,
                     const Headers&headers,
                     const std::string&method)
{
	throw eckit::SeriousBug("URLHandle: eckit not compiled with CURL support");
}

URLHandle::~URLHandle() {
	NOTIMP;
}

Length URLHandle::openForRead() {
	NOTIMP;
}


void URLHandle::openForWrite(const Length&)
{
	NOTIMP;
}

void URLHandle::openForAppend(const Length&)
{
	NOTIMP;
}

long URLHandle::read(void* buffer, long length)
{
	NOTIMP;
}

long URLHandle::write(const void* buffer, long length)
{
	NOTIMP;
}

void URLHandle::close()
{
	NOTIMP;
}

#endif


//-----------------------------------------------------------------------------

} // namespace eckit


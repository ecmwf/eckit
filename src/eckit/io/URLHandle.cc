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

#include <curl/curl.h>

//-----------------------------------------------------------------------------

namespace eckit {

#define _(a) call(#a, a)


//-----------------------------------------------------------------------------

ClassSpec URLHandle::classSpec_ = {&DataHandle::classSpec(), "URLHandle",};
Reanimator<URLHandle> URLHandle::reanimator_;

#define INITIAL_BUFFER_SIZE 1024

// curl_global_init(CURL_GLOBAL_DEFAULT);

void URLHandle::print(std::ostream&s) const
{
	s << "URLHandle[url=" << url_ << ']';
}

void URLHandle::encode(Stream& s) const
{
	DataHandle::encode(s);
	s << url_;
}

URLHandle::URLHandle(Stream& s):
	DataHandle(s)
{
	s >> url_;
}



URLHandle::URLHandle(const std::string& url):

	url_(url)
{
}


Length URLHandle::openForRead()
{
	curl_  = curl_easy_init();
	ASSERT(!curl_);

	errors_ = 0;

	const char* verbose = "1";

	_(curl_easy_setopt(curl_, CURLOPT_VERBOSE, verbose));

	_(curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str()));

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

	return 0;
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
	if (curl_)
		curl_easy_cleanup(curl_);

	if (chunk_)
		curl_slist_free_all(chunk_);

	curl_  = NULL;
	chunk_ = NULL;
	pos_   = 0;
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
		errors_++;
	}
}


void URLHandle::call(const char* what, CURLMcode code)
{
	// curl_error_ = code;
	if (code != CURLE_OK)
	{
		std::ostringstream oss;
		oss << what << " failed: " << curl_multi_strerror(code);
		errors_++;
	}
}




void URLHandle::waitForData(size_t size)
{
	fd_set fdr, fdw, fdx;
	struct timeval timeout;

	if ((!active_) || (pos_ > size))
		return;

	do {
		int maxfd = -1;
		long time = -1;

		FD_ZERO(&fdr);
		FD_ZERO(&fdw);
		FD_ZERO(&fdx);

		_(curl_multi_timeout(multi_handle_, &time));
		if (time >= 0) {
			timeout.tv_sec = time / 1000;
			if (timeout.tv_sec > 1)
				timeout.tv_sec = 1;
			else
				timeout.tv_usec = (time % 1000) * 1000;
		}
		else
		{
			timeout.tv_sec = 60;
			timeout.tv_usec = 0;
		}

		_(curl_multi_fdset(multi_handle_, &fdr, &fdw, &fdx, &maxfd));

		if (::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout) == -1)
		{
			lastError_ = "select";
			errors_++;
		}
		else {
			_(curl_multi_perform(multi_handle_, &active_));
		}

	} while (active_ && (pos_ < size));

}



size_t URLHandle::transferRead(void *ptr, size_t size)
{
	waitForData(size);

	if (pos_ == 0)
		return 0; /* EOF */

	if (pos_ < size)
		size = pos_;

	memcpy(ptr, buffer_, size);

	if (pos_ - size <= 0)
	{
		pos_ = 0;
	}
	else
	{
		memmove(buffer_, buffer_ + size, pos_ - size);
		pos_ -= size;
	}

	return size;
}

size_t URLHandle::writeCallback( void *ptr, size_t size, size_t nmemb, void *userdata) {
	return reinterpret_cast<URLHandle*>(userdata)->transferWrite(ptr, size, nmemb);
}


size_t URLHandle::transferWrite( void *ptr, size_t size, size_t nmemb)
{
	size *= nmemb;

	if (buffer_ == NULL)
	{
		len_    = INITIAL_BUFFER_SIZE;
		buffer_ = (char*)malloc(len_);
	}

	if (pos_ + size > len_) {

		while (pos_ + size > len_) {
			len_ *= 2;
		}

		buffer_ = (char*)realloc(buffer_, len_);
		if (!buffer_)
		{
			errors_++;
			return 0;
		}
	}

	memcpy(buffer_ + pos_, ptr, size);
	pos_ += size;

	return size;
}

size_t URLHandle::headersCallback( void *ptr, size_t size, size_t nmemb, void *userdata) {
	return reinterpret_cast<URLHandle*>(userdata)->receiveHeaders(ptr, size, nmemb);
}


size_t URLHandle::receiveHeaders( void *ptr, size_t size, size_t nmemb)
{

	char* p    = (char*)ptr;
	char* q    = (char*)ptr;
	int  count = size * nmemb;

	while (count-- > 0) {
		if (*p == ':') {
			if (q == (char*)ptr) {
				char *s = q;
				*p = 0;
				q = p;
				while (count-- > 0) {
					if (*p == '\r') {
						*p = 0;
						receivedHeaders_[s] = q;
					}
					p++;
				}

				*q = ':';
			}
		}
		p++;
	}

	return size * nmemb;

}


long long URLHandle::transferStart()
{


	if (!multi_handle_){
		multi_handle_ = curl_multi_init();
	}


	pos_  = 0;
	_(curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION , &headersCallback));
	_(curl_easy_setopt(curl_, CURLOPT_HEADERDATA , this));

	_(curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &writeCallback));
	_(curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this));

	_(curl_multi_add_handle(multi_handle_, curl_));

	_(curl_multi_perform(multi_handle_, &active_));

	// if((api->pos == 0) && (!api->active)) {
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
}

//-----------------------------------------------------------------------------

} // namespace eckit


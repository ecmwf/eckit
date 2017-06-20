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
// #undef ECKIT_HAVE_CURL

#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#endif

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec URLHandle::classSpec_ = {&DataHandle::classSpec(), "URLHandle",};
Reanimator<URLHandle> URLHandle::reanimator_;


#ifdef ECKIT_HAVE_CURL

#define _(a) call(#a, a)




static void call(const char* what, CURLcode code)
{
    // std::cout << "==> " << what << std::endl;

    if (code != CURLE_OK)
    {
        std::ostringstream oss;
        oss << what << " failed: " << curl_easy_strerror(code);
        throw SeriousBug(oss.str());
    }
    // std::cout << "<== " << what << std::endl;

}


static void call(const char* what, CURLMcode code)
{
    // std::cout << "==> " << what << std::endl;
    if (code != CURLM_OK)
    {
        std::ostringstream oss;
        oss << what << " failed: " << curl_multi_strerror(code);
        throw SeriousBug(oss.str());
    }
    // std::cout << "<== " << what << std::endl;

}
#endif

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init()
{
#ifdef ECKIT_HAVE_CURL
    _(curl_global_init(CURL_GLOBAL_DEFAULT));
#endif
}

void URLHandle::print(std::ostream&s) const
{
    s << "URLHandle[url=" << url_ << ']';
}

void URLHandle::encode(Stream&s) const
{
    NOTIMP;
    DataHandle::encode(s);
    s << url_;
    s << method_;
    s << sendHeaders_.size();
    for (Headers::const_iterator j = sendHeaders_.begin(); j != sendHeaders_.end(); ++j)
    {
        s << (*j).first;
        s << (*j).second;
    }
}

URLHandle::URLHandle(Stream&s):
    DataHandle(s),
    active_(0),
    multi_(0),
    curl_(0),
    body_(false)
{
    pthread_once(&once, init);

    s >> url_;
    s >> method_;

    size_t count;
    s >> count;
    for(size_t i = 0; i < count; ++i) {
        std::string k, v;
        s >> k;
        s >> v;
        sendHeaders_[k] = v;
    }
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
    body_(false) {
    pthread_once(&once, init);
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

    if (multi_) {
        curl_multi_cleanup(multi_);
    }

    multi_ = 0;
    curl_ = 0;

    buffer_.clear();
}


Length URLHandle::openForRead()
{
    buffer_.clear();

    curl_  = curl_easy_init();
    ASSERT(curl_);

    multi_ = curl_multi_init();
    ASSERT(multi_);

    // TODO: use resources

    _(curl_easy_setopt(curl_, CURLOPT_VERBOSE, 0L));
    _(curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str()));
    _(curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L));


#ifdef SKIP_PEER_VERIFICATION
    _(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L));
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    _(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L));
#endif

        struct curl_slist *chunk = 0;
  for (Headers::const_iterator j = sendHeaders_.begin(); j != sendHeaders_.end(); ++j)
    {
        std::ostringstream oss;
        oss << (*j).first << ": " <<  (*j).second;
        chunk = curl_slist_append(chunk, oss.str().c_str());
    }

    if (chunk) {
        _(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, chunk));
        curl_slist_free_all(chunk);

    }

    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, method_.c_str()));
    // _(curl_easy_setopt(curl_, CURLOPT_USERAGENT, "eckit/1.0"));

    _(curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION , &headersCallback));
    _(curl_easy_setopt(curl_, CURLOPT_HEADERDATA , this));

    _(curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &writeCallback));
    _(curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this));

    _(curl_multi_add_handle(multi_, curl_));
    _(curl_multi_perform(multi_, &active_));

    while (!body_) {
        waitForData();
    }


    double length;
    _(curl_easy_getinfo(curl_, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length ));
    ASSERT(length >= 0);

    return length;
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
    _(curl_easy_getinfo (curl_, CURLINFO_RESPONSE_CODE, &code));

    if (code != 200) {

        cleanup();

        std::ostringstream oss;
        oss << "URLHandle::close(" << url_ << ") returns code " << code;
        throw eckit::SeriousBug(oss.str());
    }
    cleanup();

}

void URLHandle::waitForData()
{
    fd_set fdr, fdw, fdx;
    struct timeval timeout;

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

void URLHandle::waitForData(size_t size)
{
    ASSERT(size);
    while (active_  > 0 && buffer_.length() < size) {
        waitForData();
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
    return buffer_.write(ptr, size * nmemb);
}

size_t URLHandle::headersCallback( void *ptr, size_t size, size_t nmemb, void *userdata) {
    return reinterpret_cast<URLHandle*>(userdata)->receiveHeaders(ptr, size, nmemb);
}

size_t URLHandle::receiveHeaders( void *ptr, size_t size, size_t nmemb)
{

    char* p    = (char*)ptr;
    int  count = size * nmemb;

    if (body_) {
        // We come back here after a redirect, so we must clear the previous headers
        receivedHeaders_.clear();
        body_ = false;

    }

    ASSERT(count >= 2);
    ASSERT(p[count - 1] == '\n');
    ASSERT(p[count - 2] == '\r');

    std::string line(p, p + count - 2);

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


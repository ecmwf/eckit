/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>

#include "eckit/eckit.h"

#include "eckit/io/EasyCURL.h"
#include "eckit/log/Log.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"


#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#endif


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#ifdef ECKIT_HAVE_CURL


#define _(a) call(#a, a)


static void call(const char* what, CURLcode code) {
    // std::cout << "==> " << what << std::endl;

    if (code != CURLE_OK) {
        std::ostringstream oss;
        oss << what << " failed: " << curl_easy_strerror(code);
        throw SeriousBug(oss.str());
    }
    // std::cout << "<== " << what << std::endl;
}


static void call(const char* what, CURLMcode code) {
    // std::cout << "==> " << what << std::endl;
    if (code != CURLM_OK) {
        std::ostringstream oss;
        oss << what << " failed: " << curl_multi_strerror(code);
        throw SeriousBug(oss.str());
    }
    // std::cout << "<== " << what << std::endl;
}

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    _(curl_global_init(CURL_GLOBAL_DEFAULT));
}

void EasyCURL::print(std::ostream& s) const {
    s << "EasyCURL[]";
}


EasyCURL::EasyCURL(const std::string& uri) :

    curl_(0),
    multi_(0),
    body_(false),
    activeTransfers_(0) {
    pthread_once(&once, init);
    curl_ = curl_easy_init();
    ASSERT(curl_);

    _(curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, &_headersCallback));
    _(curl_easy_setopt(curl_, CURLOPT_HEADERDATA, this));

    _(curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &_writeCallback));
    _(curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this));

    if (!uri.empty()) {
        url(uri);
    }

    multi_ = curl_multi_init();
    ASSERT(multi_);
    _(curl_multi_add_handle(multi_, curl_));
}

EasyCURL::~EasyCURL() {
    curl_multi_remove_handle(multi_, curl_);
    curl_easy_cleanup(curl_);
    curl_multi_cleanup(multi_);
}

// ============================

void EasyCURL::verbose(bool on) {
    _(curl_easy_setopt(curl_, CURLOPT_VERBOSE, on ? 1L : 0L));
}

void EasyCURL::followLocation(bool on) {
    _(curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, on ? 1L : 0L));
}

void EasyCURL::sslVerifyPeer(bool on) {
    _(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, on ? 1L : 0L));
}

void EasyCURL::sslVerifyHost(bool on) {
    _(curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, on ? 1L : 0L));
}

// ============================

void EasyCURL::url(const std::string& value) {
    uri_ = value;
    _(curl_easy_setopt(curl_, CURLOPT_URL, value.c_str()));
}

void EasyCURL::userAgent(const std::string& value) {
    _(curl_easy_setopt(curl_, CURLOPT_USERAGENT, value.c_str()));
}

void EasyCURL::customRequest(const std::string& value) {
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, value.c_str()));
}

// ============================

int EasyCURL::responseCode() const {
    int code;
    _(curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code));
    return code;
}

unsigned long long EasyCURL::contentLength() {

    // This will read the headers
    while (!body_) {
        waitForData();

        // Does not seems to work automaticalyy
        if (responseCode() == 301) {
            body_ = false;
        }
    }

    double length;
    _(curl_easy_getinfo(curl_, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length));
    if (length < 0) {

        if (responseCode() != 200) {
            std::ostringstream oss;
            oss << "EasyCURL(" << uri_ << ") returns code " << responseCode();
            throw eckit::SeriousBug(oss.str());
        }

        std::ostringstream oss;
        oss << "EasyCURL(" << uri_ << ") contentLength returned by server";
        throw eckit::SeriousBug(oss.str());
    }
    return length;
}

// ============================

void EasyCURL::headers(const Headers& headers) {

    struct curl_slist* chunk = 0;
    for (Headers::const_iterator j = headers.begin(); j != headers.end(); ++j) {
        std::ostringstream oss;
        oss << (*j).first << ": " << (*j).second;
        chunk = curl_slist_append(chunk, oss.str().c_str());
    }

    if (chunk) {
        _(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, chunk));
        curl_slist_free_all(chunk);
    }
}


size_t EasyCURL::headersCallback(const void* ptr, size_t size) {
    char* p = (char*)ptr;

    if (body_) {

        std::cout << "EasyCURL::headersCallback " << std::endl;

        // We come back here after a redirect, so we must clear the previous headers
        headers_.clear();
        body_ = false;
    }

    ASSERT(size >= 2);
    ASSERT(p[size - 1] == '\n');
    ASSERT(p[size - 2] == '\r');

    std::string line(p, p + size - 2);

    if (line.empty()) {
        body_ = true;
    }
    else {
        std::vector<std::string> v;

        Tokenizer parse(":");

        parse(line, v);
        if (v.size() == 2) {
            headers_[StringTools::lower(v[0])] = StringTools::trim(v[1]);
        }
    }
    return size;
}


size_t EasyCURL::_writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURL*>(userdata)->writeCallback(ptr, size * nmemb);
}


size_t EasyCURL::_headersCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURL*>(userdata)->headersCallback(ptr, size * nmemb);
}

void EasyCURL::waitForData() {
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
    else {
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;
    }

    _(curl_multi_fdset(multi_, &fdr, &fdw, &fdx, &maxfd));

    SYSCALL(::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout));
    _(curl_multi_perform(multi_, &activeTransfers_));
}


size_t EasyCURL::activeTransfers() const {
    return activeTransfers_;
}

#else

EasyCURL::EasyCURL(const std::string&) {
    NOTIMP;
}

EasyCURL::~EasyCURL() {}
void EasyCURL::followLocation(bool) {}
unsigned long long EasyCURL::contentLength() {
    return 0;
}
void EasyCURL::waitForData() {}
size_t EasyCURL::headersCallback(const void* ptr, size_t size) {
    return 0;
}
int EasyCURL::responseCode() const {
    return 500;
}
size_t EasyCURL::activeTransfers() const {
    return 0;
}
void EasyCURL::print(std::ostream& s) const {}
void EasyCURL::url(const std::string& value) {}

#endif /* ECKIT_HAVE_CURL */

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

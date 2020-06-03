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
#include "eckit/parser/JSONParser.h"


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
static CURLM *multi = 0;

static void init() {
    _(curl_global_init(CURL_GLOBAL_DEFAULT));
    multi = curl_multi_init();
}

void EasyCURL::print(std::ostream& s) const {
    s << "EasyCURL[]";
}

// ===========================================================

class EasyCURLResponseImp : public eckit::Counted {
public:
    EasyCURLResponseImp(bool stream);

    size_t writeCallback(const void *ptr, size_t size);
    size_t headersCallback(const void *ptr, size_t size);


    const EasyCURLHeaders& headers();
    const void* body(size_t& size);
    std::string body();
    Value json();
    unsigned long long contentLength();
    size_t read(void* ptr, size_t size);
    int waitForData();
    void ensureHeaders();

    bool stream_;
    long code_;
    bool body_;

    EasyCURLHeaders headers_;
    std::unique_ptr<MemoryHandle> handle_;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const EasyCURLResponseImp& c) {
        c.print(s);
        return s;
    }
};

EasyCURLResponseImp::EasyCURLResponseImp(bool stream):
    stream_(stream),
    body_(false),
    code_(0) {
}

std::string EasyCURLResponseImp::body() {
    if (!handle_) {
        return "";
    }
    size_t size = handle_->size();
    const char* p = reinterpret_cast<const char*>(handle_->data());
    return std::string(p, p + size);
}


Value EasyCURLResponseImp::json() {
    return JSONParser::decodeString(body());
}

void EasyCURLResponseImp::ensureHeaders() {
    // std::cout << "ensureHeaders " << *this << std::endl;

    while (!body_) {
        ASSERT(stream_);
        waitForData();
    }
}

void EasyCURLResponseImp::print(std::ostream& s) const {
    s << "EasyCURLResponseImp[" << this
      << ",stream=" << stream_
      << ",body=" << body_
      << ",code=" << code_
      << "]";

}

unsigned long long EasyCURLResponseImp::contentLength() {
    ensureHeaders();
    std::cout << "<<<---->>>>> " << headers_ << std::endl;

    // curl_off_t size;
    // _(curl_easy_getinfo(curl_, CURLINFO_SIZE_DOWNLOAD_T, &size));
    // ASSERT(size >= 0);
    // return size;
    NOTIMP;
}

size_t EasyCURLResponseImp::read(void* ptr, size_t size) {
    ASSERT(stream_);
    NOTIMP;
}

int EasyCURLResponseImp::waitForData()
{
    fd_set fdr, fdw, fdx;
    struct timeval timeout;

    int maxfd = -1;
    long time = -1;

    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_ZERO(&fdx);

    _(curl_multi_timeout(multi, &time));

    if (time >= 0) {
        timeout.tv_sec = time / 1000;
        if (timeout.tv_sec > 1) {
            timeout.tv_sec = 1;
        }
        else {
            timeout.tv_usec = (time % 1000) * 1000;
        }
    }
    else
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }

    _(curl_multi_fdset(multi, &fdr, &fdw, &fdx, &maxfd));

    SYSCALL (::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout));

    int active = 0;
    _(curl_multi_perform(multi, &active));

    return active;

}

size_t EasyCURLResponseImp::writeCallback(const void *ptr, size_t size) {
    // std::cout << "EasyCURLResponseImp::writeCallback(" << *this << ")" << std::endl;
    if (!handle_) {
        handle_.reset(new MemoryHandle(1024 * 4, true));
        handle_->openForWrite(0);
    }
    return handle_->write(ptr, size);
}

size_t EasyCURLResponseImp::headersCallback(const void *ptr, size_t size) {
    // std::cout << "EasyCURLResponseImp::headersCallback(" << *this << ")" << std::endl;


    char* p = (char*)ptr;

    ASSERT(!body_);

    ASSERT(size >= 2);
    ASSERT(p[size - 1] == '\n');
    ASSERT(p[size - 2] == '\r');

    std::string line(p, p + size - 2);
    // std::cout << "EasyCURLResponseImp::headersCallback(" << line << ")" << std::endl;

    if (line.empty()) {
        body_ = true;
        // std::cout << "EasyCURLResponseImp::headersCallback(body)" << std::endl;
    }
    else {
        std::vector<std::string> v;

        Tokenizer parse(":");

        parse(line, v);
        if (v.size() == 2) {
            headers_[StringTools::lower(v[0])] = StringTools::trim(v[1]);
        }
    }
    // std::cout << "EasyCURLResponseImp::headersCallback(" << *this << ")" << std::endl;

    return size;
}


// ===========================================================

EasyCURLResponse::EasyCURLResponse(EasyCURLResponseImp* imp):
    imp_(imp) {
    imp_->attach();
    // std::cout << "Response " << *imp << std::endl;
}

EasyCURLResponse::~EasyCURLResponse() {
    imp_->detach();
}

EasyCURLResponse& EasyCURLResponse::operator=(const eckit::EasyCURLResponse &other) {
    if (imp_ != other.imp_) {
// std::cout << "EasyCURLResponse::assign " << *imp_ << std::endl;
        imp_->detach();
        imp_ = other.imp_;
        imp_->attach();
        // std::cout << "EasyCURLResponse::assign " << *imp_ << std::endl;
    }
    return *this;
}

Value EasyCURLResponse::json() const {
    return imp_->json();
}

std::string EasyCURLResponse::body() const {
    return imp_->body();
}

const EasyCURLHeaders& EasyCURLResponse::headers() const {
    imp_->ensureHeaders();
    return imp_->headers_;
}

unsigned long long EasyCURLResponse::contentLength() const {
    return imp_->contentLength();
}

size_t EasyCURLResponse::read(void* ptr, size_t size) const {
    return imp_->read(ptr, size);
}

// ===========================================================

EasyCURL::EasyCURL():
    curl_(nullptr),
    chunks_(nullptr),
    stream_(false)
{

    pthread_once(&once, init);

    curl_ = curl_easy_init();
    ASSERT(curl_);

    // multi_ = curl_multi_init();
    // ASSERT(multi_);
    // _(curl_multi_add_handle(multi_, curl_));
}

EasyCURL::~EasyCURL() {
    clearStream();
    curl_slist_free_all(chunks_);
    // curl_multi_remove_handle(multi_, curl_);
    curl_easy_cleanup(curl_);
    // curl_multi_cleanup(multi_);
}

void EasyCURL::clearStream() {
    if (stream_) {
        curl_multi_remove_handle(multi, curl_);
        stream_ = false;
    }
}

// ============================

void EasyCURL::verbose(bool on) {
    std::cout << "EasyCURL::verbose(" << on << ")" << std::endl;
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

EasyCURLResponse EasyCURL::request(const std::string& url, bool stream) {

    clearStream();


    EasyCURLResponseImp* r = new EasyCURLResponseImp(stream);
    try {
        // _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, method));
        _(curl_easy_setopt(curl_, CURLOPT_URL, url.c_str()));

        _(curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, &_headersCallback));
        _(curl_easy_setopt(curl_, CURLOPT_HEADERDATA, r));

        _(curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &_writeCallback));
        _(curl_easy_setopt(curl_, CURLOPT_WRITEDATA, r));

        if (stream) {
            // std::cout << "EasyCURL::request stream" << std::endl;
            _(curl_multi_add_handle(multi, curl_));
            stream_ = true;
            r->ensureHeaders();
        }
        else {
            // std::cout << "EasyCURL::request direct" << std::endl;
            _(curl_easy_perform(curl_));
            _(curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &r->code_));
        }
    }
    catch (...) {
        delete r;
        throw;
    }

    return EasyCURLResponse(r);

}

EasyCURLResponse EasyCURL::GET(const std::string& url, bool stream) {
    std::cout << "EasyCURL::GET(" << url << ")" << std::endl;
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L));

    return request(url, stream);
}

EasyCURLResponse EasyCURL::HEAD(const std::string& url) {
    std::cout << "EasyCURL::HEAD(" << url << ")" << std::endl;
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(curl_, CURLOPT_NOBODY, 1L));
    return request(url);
}

EasyCURLResponse EasyCURL::POST(const std::string& url, const std::string& data) {
    std::cout << "EasyCURL::POST(" << url << "," << data << ")" << std::endl;
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(curl_, CURLOPT_POST, 1L));
    _(curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str()));
    return request(url);
}

EasyCURLResponse EasyCURL::PUT(const std::string& url, const std::string& data) {
    std::cout << "EasyCURL::PUT(" << url << ")" << std::endl;
    NOTIMP;
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(curl_, CURLOPT_PUT, 1L));
    return request(url);
}

EasyCURLResponse EasyCURL::DELETE(const std::string& url) {
    std::cout << "EasyCURL::DELETE(" << url << ")" << std::endl;
    _(curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE"));
    return request(url);
}


void EasyCURL::userAgent(const std::string& value) {
    std::cout << "EasyCURL::userAgent(" << value << ")" << std::endl;
    _(curl_easy_setopt(curl_, CURLOPT_USERAGENT, value.c_str()));
}

size_t EasyCURL::_writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURLResponseImp*>(userdata)->writeCallback(ptr, size * nmemb);
}

size_t EasyCURL::_headersCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURLResponseImp*>(userdata)->headersCallback(ptr, size * nmemb);
}

void EasyCURL::headers(const EasyCURLHeaders& headers) {

    curl_slist_free_all(chunks_);

    for (auto j = headers.begin(); j != headers.end(); ++j) {
        std::ostringstream oss;
        std::cout << "EasyCURL::headers(" << (*j).first << ": " << (*j).second << ")" << std::endl;
        oss << (*j).first << ": " << (*j).second;
        chunks_ = curl_slist_append(chunks_, oss.str().c_str());
    }

    if (chunks_) {
        _(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, chunks_));
    }
}

// ============================
#if 0
int EasyCURL::responseCode() {
    int code;
    ensureHeaders();
    _(curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code));
    return code;
}


void EasyCURL::ensureHeaders() {
    // This will read the headers
    while (!body_) {
        waitForData();

        // Does not seems to work automaticalyy
        // if (responseCode() == 301) {
        //     body_ = false;
        //     headers_.clear();
        //     buffer_.close();
        // }
    }
}

void EasyCURL::ensureBody() {
    ensureHeaders();
    if (body_) {
        buffer_.openForWrite(0);
        size_ = 0;
        while (activeTransfers()) {
            waitForData();
        }
        buffer_.close();
    }
}

const EasyCURL::Headers& EasyCURL::headers() {
    ensureHeaders();
    return headers_;
}

const void* EasyCURL::body(size_t& size) {
    ensureBody();
    size = size_;
    return buffer_.data();
}

std::string EasyCURL::body() {
    size_t size;
    const char *p = reinterpret_cast<const char*>(body(size));
    return std::string(p, p + size);
}

Value EasyCURL::json() {
    std::cout << "BODY " << body() << std::endl;
    return JSONParser::decodeString(body());
}


unsigned long long EasyCURL::contentLength() {

    ensureHeaders();

    curl_off_t length;
    _(curl_easy_getinfo(curl_, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &length));
    if (length < 0) {

        if (responseCode() != 200) {
            std::ostringstream oss;
            oss << "EasyCURL(" << uri_ << ") returns code " << responseCode();
            throw eckit::SeriousBug(oss.str());
        }

        std::ostringstream oss;
        oss << "EasyCURL(" << uri_ << ") contentLength returned by server " << length;
        throw eckit::SeriousBug(oss.str());
    }
    return length;
}

// ============================



size_t EasyCURL::headersCallback(const void* ptr, size_t size) {
    char* p = (char*)ptr;

    if (body_) {

        std::cout << "EasyCURL::headersCallback " << std::endl;

        // We come back here after a redirect, so we must clear the previous headers
        headers_.clear();
        body_ = false;
        buffer_.close();
    }

    ASSERT(size >= 2);
    ASSERT(p[size - 1] == '\n');
    ASSERT(p[size - 2] == '\r');

    std::string line(p, p + size - 2);

    std::cout << "HEADER [" << line << "]" << std::endl;

    if (line.empty()) {
        body_ = true;
        buffer_.openForWrite(0);
        size_ = 0;
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



void EasyCURL::waitForData() {


    _(curl_easy_perform(curl_));
    return;


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

    std::cout << "select " << maxfd << " " << timeout.tv_sec << " " <<  timeout.tv_usec << std::endl;

    SYSCALL(::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout));
    _(curl_multi_perform(multi_, &activeTransfers_));
}


size_t EasyCURL::activeTransfers() const {
    return activeTransfers_;
}


size_t EasyCURL::writeCallback(const void* ptr, size_t size) {
    size = buffer_.write(ptr, size);
    size_ += size;
    return size;
}
#endif


#else

EasyCURL::EasyCURL() {
    NOTIMP;
}

EasyCURL::~EasyCURL() {
}

#endif /* ECKIT_HAVE_CURL */

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

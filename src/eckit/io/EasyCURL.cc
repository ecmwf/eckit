/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <arpa/inet.h>
#include <unistd.h>
#include <curl/curl.h>

#include "eckit/eckit.h"

#include "eckit/io/EasyCURL.h"
#include "eckit/log/Log.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/parser/JSONParser.h"
#include "eckit/io/CircularBuffer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


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


class CURLHandle : public eckit::Counted {
public:
    CURL *curl_;
    curl_slist* chunks_;

    CURLHandle():
        curl_(nullptr),
        chunks_(nullptr)
    {
        pthread_once(&once, init);
        curl_ = curl_easy_init();
        ASSERT(curl_);
    }

    ~CURLHandle() {
        curl_slist_free_all(chunks_);
        curl_easy_cleanup(curl_);
    }
};

// ===========================================================

class EasyCURLResponseImp : public eckit::Counted {
public:
    EasyCURLResponseImp(const std::string& url, CURLHandle *curl);
    ~EasyCURLResponseImp();

    virtual void perform() = 0;

    virtual size_t writeCallback(const void *ptr, size_t size) = 0;

    virtual size_t headersCallback(const void *ptr, size_t size);

    virtual const EasyCURLHeaders& headers() {
        ensureHeaders();
        return headers_;
    }

    virtual std::string body() = 0;
    virtual unsigned long long contentLength() = 0;
    virtual size_t read(void* ptr, size_t size) = 0;
    virtual void ensureHeaders() = 0;

    std::string url_;
    CURLHandle* ch_;
    long code_;
    bool body_;

    EasyCURLHeaders headers_;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const EasyCURLResponseImp& c) {
        c.print(s);
        return s;
    }

    static size_t _writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t _headersCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
};



class EasyCURLResponseDirect : public EasyCURLResponseImp {
public:
    std::unique_ptr<MemoryHandle> handle_;
    EasyCURLResponseDirect(const std::string& url, CURLHandle *curl): EasyCURLResponseImp(url, curl) {};

    virtual void perform() {
        _(curl_easy_setopt(ch_->curl_, CURLOPT_URL, url_.c_str()));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_HEADERFUNCTION, &_headersCallback));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_HEADERDATA, this));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_WRITEFUNCTION, &_writeCallback));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_WRITEDATA, this));
        _(curl_easy_perform(ch_->curl_));
        _(curl_easy_getinfo(ch_->curl_, CURLINFO_RESPONSE_CODE, &code_));


        if (code_ == 301) { // Move permanently
            // CURL's built-in redirect is not what we want
            char *url = NULL;
            _(curl_easy_getinfo(ch_->curl_, CURLINFO_REDIRECT_URL, &url));
            _(curl_easy_setopt(ch_->curl_, CURLOPT_URL, url_.c_str()));

            if (url) {
                body_ = false;
                handle_.reset(0);
                _(curl_easy_setopt(ch_->curl_, CURLOPT_URL, url));
                _(curl_easy_perform(ch_->curl_));
                _(curl_easy_getinfo(ch_->curl_, CURLINFO_RESPONSE_CODE, &code_));
            }

        }

    }

    virtual std::string body() {
        if (!handle_) {
            return "";
        }
        size_t size = handle_->size();
        const char* p = reinterpret_cast<const char*>(handle_->data());
        return std::string(p, p + size);
    }


    size_t writeCallback(const void *ptr, size_t size) {

        std::cout << "EasyCURLResponseImpDirect::writeCallback(" << size << ")" << std::endl;

        if (!handle_) {
            handle_.reset(new MemoryHandle(1024 * 64, true));
            handle_->openForWrite(0);
        }
        return handle_->write(ptr, size);
    }

    virtual unsigned long long contentLength() {
        NOTIMP;
    }

    virtual size_t read(void* ptr, size_t size) {
        NOTIMP;
    }

    virtual void ensureHeaders() {
    }

};

class EasyCURLResponseStream : public EasyCURLResponseImp {
public:
    CircularBuffer buffer_;

    EasyCURLResponseStream(const std::string& url, CURLHandle *curl): EasyCURLResponseImp(url, curl) {}
    ~EasyCURLResponseStream() {
        _(curl_multi_remove_handle(multi, ch_->curl_));
    }

    virtual void perform() {
        _(curl_easy_setopt(ch_->curl_, CURLOPT_URL, url_.c_str()));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_HEADERFUNCTION, &_headersCallback));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_HEADERDATA, this));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_WRITEFUNCTION, &_writeCallback));
        _(curl_easy_setopt(ch_->curl_, CURLOPT_WRITEDATA, this));

        _(curl_multi_add_handle(multi, ch_->curl_));
        int active = 0;
        _(curl_multi_perform(multi, &active));
        _(curl_easy_getinfo(ch_->curl_, CURLINFO_RESPONSE_CODE, &code_));


        // std::cout << "EasyCURLResponseImpStream::perform " << active << " " << buffer_.length() << std::endl;
        // std::cout << "EasyCURLResponseImpStream::perform " << *this << std::endl;
    }

    size_t writeCallback(const void *ptr, size_t size) {
        // std::cout << "EasyCURLResponseImpStream::writeCallback(" << size << ")" << std::endl;
        return buffer_.write(ptr, size);
    }

    virtual std::string body() {
        NOTIMP;
    }

    virtual unsigned long long contentLength() {
        ensureHeaders();
        auto j = headers_.find("content-length");
        if (j != headers_.end()) {
            return Translator<std::string, unsigned long long>()((*j).second);
        }
        throw eckit::SeriousBug("EasyCURLResponseStream: cannot establish contentLength");
    }

    int waitForData() {
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

        if (maxfd == -1) {
            timeout = { 0, 100 * 1000 };
            select(0, NULL, NULL, NULL, &timeout);
        }
        else {
            SYSCALL (::select(maxfd + 1, &fdr, &fdw, &fdx, &timeout));
        }

        int active = 0;
        _(curl_multi_perform(multi, &active));

        if (active == 0) {
            _(curl_easy_getinfo(ch_->curl_, CURLINFO_RESPONSE_CODE, &code_));
        }

        return active;
    }

    virtual size_t read(void* ptr, size_t size) {

        while (buffer_.length() < size) {
            if (waitForData() == 0) {
                break;
            }
        }

        return buffer_.read(ptr, size);

    }

    virtual void ensureHeaders() {
        while (!body_) {
            if (waitForData() == 0) {
                break;
            }
        }
    }

};

EasyCURLResponseImp::EasyCURLResponseImp(const std::string& url, CURLHandle *curl):
    url_(url),
    ch_(curl),
    body_(false),
    code_(0) {
    ch_->attach();
}

EasyCURLResponseImp::~EasyCURLResponseImp() {
    ch_->detach();
}

void EasyCURLResponseImp::print(std::ostream& s) const {
    s << "EasyCURLResponseImp[" << this
      << ",body=" << body_
      << ",code=" << code_
      << "]";
}

size_t EasyCURLResponseImp::headersCallback(const void *ptr, size_t size) {

    char* p = (char*)ptr;

    ASSERT(!body_);

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

// ===========================================================

class EasyCURLHandle : public DataHandle {
public:
    EasyCURLHandle(EasyCURLResponseImp* imp);
    ~EasyCURLHandle();

private:
    EasyCURLResponseImp* imp_;

    virtual void print(std::ostream& s) const;
    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);
    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    virtual Length size();
};


EasyCURLHandle::EasyCURLHandle(EasyCURLResponseImp* imp):
    imp_(imp) {
    imp_->attach();
}

EasyCURLHandle::~EasyCURLHandle() {
    imp_->detach();
}

void EasyCURLHandle::print(std::ostream& s) const {
    s << "EasyCURLHandle[]";
}

Length EasyCURLHandle::openForRead() {
    return size();
}

Length EasyCURLHandle::size() {
    return imp_->contentLength();
}

void EasyCURLHandle::openForWrite(const Length&) {
    NOTIMP;
}

void EasyCURLHandle::openForAppend(const Length&) {
    NOTIMP;
}

long EasyCURLHandle::read(void* ptr, long size) {
    return imp_->read(ptr, size);
}

long EasyCURLHandle::write(const void*, long) {
    NOTIMP;
}

void EasyCURLHandle::close() {
    ASSERT(imp_->code_ == 200);
}

// ===========================================================

EasyCURLResponse::EasyCURLResponse(EasyCURLResponseImp* imp):
    imp_(imp) {
    imp_->attach();
}

EasyCURLResponse::~EasyCURLResponse() {
    imp_->detach();
}

EasyCURLResponse& EasyCURLResponse::operator=(const eckit::EasyCURLResponse &other) {
    if (imp_ != other.imp_) {
        imp_->detach();
        imp_ = other.imp_;
        imp_->attach();
    }
    return *this;
}

Value EasyCURLResponse::json() const {
    return JSONParser::decodeString(imp_->body());
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

DataHandle* EasyCURLResponse::dataHandle() const {
    return new EasyCURLHandle(imp_);
}

// ===========================================================

EasyCURL::EasyCURL():
    ch_(new CURLHandle()) {
    ch_->attach();
}

EasyCURL::~EasyCURL() {
    ch_->detach();
}

// ============================

void EasyCURL::verbose(bool on) {
    std::cout << "EasyCURL::verbose(" << on << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_VERBOSE, on ? 1L : 0L));
}

void EasyCURL::followLocation(bool on) {
    _(curl_easy_setopt(ch_->curl_, CURLOPT_FOLLOWLOCATION, on ? 1L : 0L));
}

void EasyCURL::sslVerifyPeer(bool on) {
    _(curl_easy_setopt(ch_->curl_, CURLOPT_SSL_VERIFYPEER, on ? 1L : 0L));
}

void EasyCURL::sslVerifyHost(bool on) {
    _(curl_easy_setopt(ch_->curl_, CURLOPT_SSL_VERIFYHOST, on ? 1L : 0L));
}

// ============================

EasyCURLResponse EasyCURL::request(const std::string& url, bool stream) {


    EasyCURLResponseImp* r = 0;

    if (stream)  {
        r = new EasyCURLResponseStream(url, ch_);
    }
    else {
        r = new EasyCURLResponseDirect(url, ch_);
    }

    try {
        r->perform();
    }
    catch (...) {
        delete r;
        throw;
    }

    return EasyCURLResponse(r);

}

EasyCURLResponse EasyCURL::GET(const std::string& url, bool stream) {
    std::cout << "EasyCURL::GET(" << url << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(ch_->curl_, CURLOPT_HTTPGET, 1L));

    return request(url, stream);
}

EasyCURLResponse EasyCURL::HEAD(const std::string& url) {
    std::cout << "EasyCURL::HEAD(" << url << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(ch_->curl_, CURLOPT_NOBODY, 1L));
    return request(url);
}

EasyCURLResponse EasyCURL::POST(const std::string& url, const std::string& data) {
    std::cout << "EasyCURL::POST(" << url << "," << data << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(ch_->curl_, CURLOPT_POST, 1L));
    _(curl_easy_setopt(ch_->curl_, CURLOPT_POSTFIELDS, data.c_str()));
    return request(url);
}

EasyCURLResponse EasyCURL::PUT(const std::string& url, const std::string& data) {
    std::cout << "EasyCURL::PUT(" << url << ")" << std::endl;
    NOTIMP;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_CUSTOMREQUEST, NULL));
    _(curl_easy_setopt(ch_->curl_, CURLOPT_PUT, 1L));
    return request(url);
}

EasyCURLResponse EasyCURL::DELETE(const std::string& url) {
    std::cout << "EasyCURL::DELETE(" << url << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_CUSTOMREQUEST, "DELETE"));
    return request(url);
}


void EasyCURL::userAgent(const std::string& value) {
    std::cout << "EasyCURL::userAgent(" << value << ")" << std::endl;
    _(curl_easy_setopt(ch_->curl_, CURLOPT_USERAGENT, value.c_str()));
}

size_t EasyCURLResponseImp::_writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURLResponseImp*>(userdata)->writeCallback(ptr, size * nmemb);
}

size_t EasyCURLResponseImp::_headersCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return reinterpret_cast<EasyCURLResponseImp*>(userdata)->headersCallback(ptr, size * nmemb);
}

void EasyCURL::headers(const EasyCURLHeaders& headers) {

    curl_slist_free_all(ch_->chunks_);

    for (auto j = headers.begin(); j != headers.end(); ++j) {
        std::ostringstream oss;
        std::cout << "EasyCURL::headers(" << (*j).first << ": " << (*j).second << ")" << std::endl;
        oss << (*j).first << ": " << (*j).second;
        ch_->chunks_ = curl_slist_append(ch_->chunks_, oss.str().c_str());
    }

    if (ch_->chunks_) {
        _(curl_easy_setopt(ch_->curl_, CURLOPT_HTTPHEADER, ch_->chunks_));
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

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
/// @date   June 2017

#ifndef eckit_io_EasyCURL_h
#define eckit_io_EasyCURL_h

#include <map>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/MemoryHandle.h"


#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#else
typedef int CURL;
typedef int CURLM;
#endif


namespace eckit {

class Value;

typedef std::map<std::string, std::string> EasyCURLHeaders;
class URLException : public Exception {
        int code_;
    public:
        URLException(const std::string& what, int code):
            Exception(what), code_(code) {}
        int code() const { return code_; }
    };

//----------------------------------------------------------------------------------------------------------------------

class EasyCURLResponseImp;

class EasyCURLResponse {
    mutable EasyCURLResponseImp* imp_;

public:
    EasyCURLResponse(EasyCURLResponseImp*);
    ~EasyCURLResponse();

    EasyCURLResponse(const EasyCURLResponse&);
    EasyCURLResponse& operator=(const EasyCURLResponse&);

    Value json() const;
    std::string body() const;
    const EasyCURLHeaders& headers() const;

    unsigned long long contentLength() const;
    size_t read(void* ptr, size_t size) const;

};

class EasyCURL {
public:


// -- Exceptions

// -- Contructors

    EasyCURL();

// -- Destructor

    ~EasyCURL();

// -- Methods

    EasyCURLResponse GET(const std::string& url, bool stream=false);
    EasyCURLResponse HEAD(const std::string& url);
    EasyCURLResponse PUT(const std::string& url, const std::string& data);
    EasyCURLResponse POST(const std::string& url, const std::string& data);
    EasyCURLResponse DELETE(const std::string& url);

    void verbose(bool on);
    void followLocation(bool on);
    void sslVerifyPeer(bool on);
    void sslVerifyHost(bool on);
    void customRequest(const std::string&);
    void headers(const EasyCURLHeaders& headers);
    void userAgent(const std::string&);

public:

    // void url(const std::string&);


    // int responseCode();
    // unsigned long long contentLength();
    // // const Headers& headers();
    // const void* body(size_t& size);
    // std::string body();

    // void waitForData();
    // size_t activeTransfers() const;

// -- Overridden methods

    // virtual void rewind();
    void print(std::ostream&) const;

    // From Streamable


// -- Class methods


private:

// -- Members

    CURL *curl_;
    curl_slist* chunks_;
    bool stream_;

    // CURLM *multi_;

    EasyCURLResponse request(const std::string& url, bool stream=false);

// -- Methods

    void clearStream();

// -- Class members

    static size_t _writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t _headersCallback(void *ptr, size_t size, size_t nmemb, void *userdata);

    friend std::ostream& operator<<(std::ostream& s, const EasyCURL& c) {
        c.print(s);
        return s;
    }


};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

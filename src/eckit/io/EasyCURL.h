/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/EasyCURL.h
// Baudouin Raoult - ECMWF Jun 17

#ifndef eckit_io_EasyCURL_h
#define eckit_io_EasyCURL_h

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"

// #undef ECKIT_HAVE_CURL

#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#else
typedef int CURL;
typedef int CURLM;
#endif

//-----------------------------------------------------------------------------

namespace eckit {


//-----------------------------------------------------------------------------

class EasyCURL {
public:

    class URLException : public Exception {
        int code_;
    public:
        URLException(const std::string& what, int code):
            Exception(what), code_(code) {}
        int code() const { return code_; }
    };

    typedef std::map<std::string, std::string> Headers;

// -- Exceptions

// -- Contructors

    EasyCURL(const std::string& url = std::string());

// -- Destructor

    virtual ~EasyCURL();

    void verbose(bool on);
    void followLocation(bool on);
    void sslVerifyPeer(bool on);
    void sslVerifyHost(bool on);


    void url(const std::string&);
    void userAgent(const std::string&);
    void customRequest(const std::string&);

    void headers(const Headers& headers);

    int responseCode() const;
    unsigned long long contentLength();

    void waitForData();
    size_t activeTransfers() const;

// -- Overridden methods

    // virtual void rewind();
    virtual void print(std::ostream&) const;

    // From Streamable


// -- Class methods


private:

// -- Members

    CURL *curl_;
    CURLM *multi_;

    std::string uri_;
    bool body_;
    Headers headers_;
    int activeTransfers_;

// -- Methods

    virtual size_t writeCallback(void *ptr, size_t size) = 0;

    virtual size_t headersCallback(const void *ptr, size_t size);



// -- Class members

    static size_t _writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t _headersCallback(void *ptr, size_t size, size_t nmemb, void *userdata);



    friend std::ostream& operator<<(std::ostream& s, const EasyCURL& c)
    { c.print(s); return s;}


};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif

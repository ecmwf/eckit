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
#include "eckit/eckit_config.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/CircularBuffer.h"

// #undef ECKIT_HAVE_CURL

#ifdef ECKIT_HAVE_CURL
#include <curl/curl.h>
#else
typedef int CURLM;
typedef int CURL;
#endif

//-----------------------------------------------------------------------------

namespace eckit {

class CircularBuffer;

//-----------------------------------------------------------------------------

class URLHandle : public DataHandle {
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

    URLHandle(const std::string&,
              const Headers& = Headers(),
              const std::string& method = "GET");

    URLHandle(Stream&);

// -- Destructor

    ~URLHandle();

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
    Headers sendHeaders_;

    int active_;
    bool body_;

    CURLM *multi_;
    CURL *curl_;

    CircularBuffer buffer_;

    Headers receivedHeaders_;

// -- Methods

    void waitForData(size_t);
    void waitForData();

    size_t transferRead(void *ptr, size_t size);
    size_t transferWrite( void *ptr, size_t size, size_t nmemb);
    size_t receiveHeaders( void *ptr, size_t size, size_t nmemb);

    static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t headersCallback( void *ptr, size_t size, size_t nmemb, void *userdata);

    long long transferStart();

    int transferEnd();
    void cleanup();

// -- Class members

    static  ClassSpec               classSpec_;
    static  Reanimator<URLHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif

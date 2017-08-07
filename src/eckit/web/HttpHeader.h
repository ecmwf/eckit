/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HttpHeader.h
// Manuel Fuentes - ECMWF Oct 96

#ifndef HttpHeader_H
#define HttpHeader_H

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/io/ResizableBuffer.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class HttpError : public Exception {

    size_t status_;

public:

    enum  {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        SEE_OTHER = 303,
        NOT_FOUND = 404,
        NOT_IMPLEMENTED = 501,
        INTERNAL_SERVER_ERROR = 500,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
    };

public:
    HttpError(size_t status, const std::string& msg = "HttpError"):
        Exception(msg),
        status_(status) {}

    size_t status() const { return status_; }
};


class HttpHeader : private eckit::NonCopyable {

public: // methods

    HttpHeader();

    ~HttpHeader();

    HttpHeader& operator=(std::map<std::string, std::string, std::less<std::string> >&);

    void length(const long);
    long contentLength() const;
    void type(const std::string& );
    void status(const long, const std::string& message = "");
    void authenticate(const std::string& );
    bool authenticated() const;
    void forward(const std::string& );
    void dontCache();
    void retryAfter(long);

    const std::string& type() const;

    const std::string& getHeader(const std::string&) const;
    void setHeader(const std::string&, const std::string&);

    void  content(const char*, long);
    const char* content() const { return content_; }

protected: // methods

    void print(std::ostream&) const;

private: // members

    std::string version_;
    long   statusCode_;
    long   contentLength_;
    std::string message_;

    struct compare {
        bool operator()(const std::string&, const std::string&) const;
    };

    typedef std::map<std::string, std::string, HttpHeader::compare> Map;

    Map header_;
    eckit::ResizableBuffer content_;

private: // methods

    friend std::ostream& operator<<(std::ostream& s, const HttpHeader& p)
    { p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif

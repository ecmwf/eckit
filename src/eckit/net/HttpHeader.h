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
/// @author Tiago Quintino

#ifndef HttpHeader_H
#define HttpHeader_H

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

namespace net {
class TCPSocket;
};

//----------------------------------------------------------------------------------------------------------------------

class HttpError : public Exception {

    int status_;

public:

    enum {
        OK                    = 200,
        CREATED               = 201,
        ACCEPTED              = 202,
        NO_CONTENT            = 204,
        SEE_OTHER             = 303,
        BAD_REQUEST           = 400,
        UNAUTHORIZED          = 401,
        NOT_FOUND             = 404,
        NOT_IMPLEMENTED       = 501,
        INTERNAL_SERVER_ERROR = 500,
    };

public:

    HttpError(int status, const std::string& msg = "HttpError") : Exception(msg), status_(status) {}

    int status() const { return status_; }
};


class HttpHeader : private eckit::NonCopyable {

public:  // methods

    HttpHeader();
    HttpHeader(net::TCPSocket&);

    ~HttpHeader();

    HttpHeader& operator=(std::map<std::string, std::string, std::less<std::string> >&);

    void length(const long);
    long contentLength() const;
    void type(const std::string&);
    void status(const long, const std::string& message = "");
    void authenticate(const std::string&);
    bool authenticated() const;
    void forward(const std::string&);
    void dontCache();
    void retryAfter(long);

    const std::string& type() const;

    const std::string& getHeader(const std::string&) const;
    void setHeader(const std::string&, const std::string&);

    void content(const char*, long);
    std::string content() const;

    void checkForStatus() const;

protected:  // methods

    void print(std::ostream&) const;

private:  // members

    std::string version_;
    long statusCode_;
    long contentLength_;
    std::string message_;
    bool received_;

    struct compare {
        bool operator()(const std::string&, const std::string&) const;
    };

    typedef std::map<std::string, std::string, HttpHeader::compare> Map;

    Map header_;
    eckit::MemoryHandle content_;

private:  // methods

    friend std::ostream& operator<<(std::ostream& s, const HttpHeader& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif

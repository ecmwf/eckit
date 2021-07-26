/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/HttpHeader.h"
#include "eckit/log/Log.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


// Check the <A HREF=http://www.ics.uci.edu/pub/ietf/http/rfc1945.html">HTTP/1.0</A> syntax
// Check the <A HREF=http://src.doc.ic.ac.uk/computing/internet/rfc/rfc2068.txt">HTTP/1.1</A> syntax

const std::string WWW_Authenticate = "WWW-Authenticate";
const std::string Authorization    = "Authorization";
const std::string Content_Type     = "Content-Type";
const std::string Content_Length   = "Content-Length";
const std::string Location         = "Location";
const std::string DefaultType      = "application/x-www-form-urlencoded";
const std::string Retry_After      = "Retry-After";

bool HttpHeader::compare::operator()(const std::string& a, const std::string& b) const {
    return strcasecmp(a.c_str(), b.c_str()) < 0;
}


HttpHeader::HttpHeader() :
    version_("HTTP/1.0"), statusCode_(HttpError::OK), contentLength_(0), received_(false) {
    header_[Content_Type]    = "text/html";
    header_["Cache-Control"] = "no-cache";
    header_["MIME-Version"]  = "1.0";
}

HttpHeader& HttpHeader::operator=(std::map<std::string, std::string, std::less<std::string> >& parsed) {
    for (std::map<std::string, std::string, std::less<std::string> >::const_iterator i = parsed.begin();
         i != parsed.end(); ++i)
        header_[(*i).first] = (*i).second;

    Map::const_iterator j = header_.find(Content_Length);

    if (j != header_.end())
        contentLength_ = atol(((*j).second).c_str());
    else
        contentLength_ = 0;

    return *this;
}

HttpHeader::~HttpHeader() {}

void HttpHeader::print(std::ostream& s) const {

    const char* CRLF = "\r\n";

    // Status line + CRLF
    s << version_ << ' ' << statusCode_ << ' ';

    if (message_.empty()) {
        switch (statusCode_) {

            case HttpError::OK:
                s << "OK";
                break;

            case HttpError::CREATED:
                s << "Created";
                break;

            case HttpError::ACCEPTED:
                s << "Accepted";
                break;

            case HttpError::NO_CONTENT:
                s << "No Content";
                break;

            case HttpError::SEE_OTHER:
                s << "See Other";
                break;

            case HttpError::NOT_FOUND:
                s << "Not Found";
                break;

            case HttpError::NOT_IMPLEMENTED:
                s << "Not Implemented";
                break;

            case HttpError::INTERNAL_SERVER_ERROR:
                s << "Internal Server Error";
                break;

            case HttpError::BAD_REQUEST:
                s << "Bad Request";
                break;

            case HttpError::UNAUTHORIZED:
                s << "Unauthorized";
                break;
        }
    }
    else {
        s << message_;
    }

    s << CRLF;


    for (Map::const_iterator i = header_.begin(); i != header_.end(); ++i) {
        s << (*i).first << ": " << (*i).second << CRLF;
    }

    if (!received_) {
        s << Content_Length << ": " << contentLength_ + content_.size() << CRLF;
    }

    s << CRLF;


    long len      = content_.size();
    const char* p = static_cast<const char*>(content_.data());
    while (len-- > 0)
        s.put(*p++);
}

void HttpHeader::forward(const std::string& s) {
    header_[Location] = s;
}

void HttpHeader::retryAfter(long s) {
    std::ostringstream oss;
    oss << s;
    header_[Retry_After] = oss.str();
    Log::debug() << "HttpHeader::retryAfter " << s << std::endl;
    Log::debug() << *this << std::endl;
}

void HttpHeader::length(const long l) {
    contentLength_ = l;
}

long HttpHeader::contentLength() const {
    return contentLength_;
}

void HttpHeader::type(const std::string& s) {
    header_[Content_Type] = s;
}

const std::string& HttpHeader::type() const {
    Map::const_iterator i = header_.find(Content_Type);

    if (i != header_.end())
        return (*i).second;

    return DefaultType;
}

void HttpHeader::status(const long code, const std::string& message) {
    statusCode_ = code;
    message_    = message;
}

void HttpHeader::authenticate(const std::string& login) {
    header_[WWW_Authenticate] = ("Basic realm=\"" + login + "\"");
    status(HttpError::UNAUTHORIZED);
}

void HttpHeader::dontCache() {}

bool HttpHeader::authenticated() const {
    Map::const_iterator i = header_.find(Authorization);

    if (i != header_.end()) {
        const char* s = (*i).second.c_str();

        while (*s != ' ' && *s != '\t')
            s++;
        while (*s == ' ' || *s == '\t')
            s++;

        unsigned char b64[256];
        for (int j = 0; j < 256; j++)
            b64[j] = 64;

        for (unsigned char c = 'A'; c <= 'Z'; c++)
            b64[c] = c - 'A';
        for (unsigned char c = 'a'; c <= 'z'; c++)
            b64[c] = c - 'a' + 26;
        for (unsigned char c = '0'; c <= '9'; c++)
            b64[c] = c - '0' + 52;

        b64[int('+')] = 62;
        b64[int('/')] = 63;

        const unsigned char* p = (const unsigned char*)s;

        std::string q;

        int n = 2;

        while (b64[*p] < 64 && b64[*(p + 1)] < 64) {
            q += char((b64[p[0]] << n) | (b64[p[1]] >> (6 - n)));
            n += 2;
            if (n == 8) {
                p++;
                n = 2;
            }
            p++;
        }

        std::cout << q << std::endl;


        Tokenizer parse(":");
        std::vector<std::string> v;
        parse(q, v);
        if (v.size() == 2 && v[0] == "mars" && v[1] == "clave") {
            Log::info() << "client authenticated " << q << " -> " << (*i).second << std::endl;
            return true;
        }

        Log::info() << "client denied " << q << " -> " << (*i).second << std::endl;
    }

    return false;
}

void HttpHeader::content(const char* p, long len) {
    content_.openForWrite(0);
    content_.write(p, len);
    content_.close();
}

void HttpHeader::setHeader(const std::string& k, const std::string& v) {
    header_[k] = v;
}

const std::string& HttpHeader::getHeader(const std::string& k) const {
    return ((HttpHeader*)this)->header_[k];
}

std::string HttpHeader::content() const {
    return content_.str();
}

//----------------------------------------------------------------------------------------------------------------------

static std::string nextLine(net::TCPSocket& socket) {
    char c;
    std::string s;

    for (;;) {
        ASSERT(socket.read(&c, 1) == 1);
        if (c == '\r') {
            ASSERT(socket.read(&c, 1) == 1);
            ASSERT(c == '\n');
            return s;
        }

        s += c;
        ASSERT(s.length() < 32768);
    }
}

HttpHeader::HttpHeader(net::TCPSocket& socket) :
    received_(true) {
    std::string line = nextLine(socket);

    size_t i = line.find_first_of(' ');
    ASSERT(i != std::string::npos);

    version_ = line.substr(0, i);

    line.erase(0, i + 1);

    i = line.find_first_of(' ');
    ASSERT(i != std::string::npos);
    statusCode_ = Translator<std::string, long>()(line.substr(0, i));

    line.erase(0, i + 1);
    message_ = line;


    line = nextLine(socket);
    while (!line.empty()) {

        size_t i = line.find_first_of(':');
        ASSERT(i != std::string::npos);

        std::string key = line.substr(0, i);
        while (!key.empty() && key[key.length() - 1] == ' ') {
            key.erase(key.length() - 1, 1);
        }

        std::string value = line.substr(i + 1, line.length());
        while (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }

        header_[key] = value;

        line = nextLine(socket);
    }
}

void HttpHeader::checkForStatus() const {
    if (statusCode_ != HttpError::OK) {
        throw HttpError(statusCode_, message_);
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

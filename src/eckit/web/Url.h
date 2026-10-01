// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult

#ifndef eckit_web_Url_h
#define eckit_web_Url_h

#include <map>
#include <memory>

#include "eckit/net/HttpHeader.h"
#include "eckit/value/Value.h"

namespace eckit {

class Url;

//----------------------------------------------------------------------------------------------------------------------

class UrlAccess {
    Url& url_;
    std::string s_;


public:

    UrlAccess(Url& url, const std::string& s) : url_(url), s_(s) {}

    operator std::string();
    operator long();

    UrlAccess& operator=(const std::string&);
    UrlAccess& operator=(long);
};

//----------------------------------------------------------------------------------------------------------------------

class Url {
public:

    Url(std::istream&);
    Url(const std::string&);

    Url(const Url&)            = delete;
    Url& operator=(const Url&) = delete;
    Url(Url&&)                 = delete;
    Url& operator=(Url&&)      = delete;

    ~Url();

    UrlAccess operator[](const std::string&);

    void erase(const std::string&);

    void set(const std::string&, const std::string&);
    const std::string& get(const std::string&);

    const std::string& method() { return method_; }


    HttpHeader& headerIn();
    HttpHeader& headerOut();

    const HttpHeader& headerIn() const;
    const HttpHeader& headerOut() const;

    std::string str() const;
    std::string name() const;


    int size() const;
    const std::string& operator[](int) const;

    bool authenticated() { return headerIn().authenticated(); }

    void authenticate(const std::string& realm = "MARS") { headerOut().authenticate(realm); }

    void status(int s, const std::string& message = "") { headerOut().status(s, message); }

    void type(const std::string& type) { headerOut().type(type); }

    void forward(const std::string& s) { headerOut().forward(s); }

    void location(const std::string& s) { headerOut().forward(s); }

    void retryAfter(long s) { headerOut().retryAfter(s); }

    void dontCache() { headerOut().dontCache(); }

    void cgiParam(std::ostream&, char sep = ' ') const;

    eckit::Value json() const;

    void remaining(const std::vector<std::string>& remaining) { remaining_ = remaining; }

    const std::vector<std::string>& remaining() const { return remaining_; }

    void streamFrom(DataHandle*, const std::string& type = "application/octet-stream");

    DataHandle* streamFrom();
    const std::string& streamType() const;

protected:  // methods

    void print(std::ostream&) const;

private:  // methods

    void parse(const std::string&, bool);
    void parse(std::istream&);

    friend std::ostream& operator<<(std::ostream& s, const Url& p) {
        p.print(s);
        return s;
    }

private:  // members

    using dict_t = std::map<std::string, std::string>;

    std::unique_ptr<DataHandle> handle_;
    std::string type_;

    dict_t dict_;

    std::vector<std::string> url_;

    HttpHeader in_;
    HttpHeader out_;

    std::string method_;

    std::vector<std::string> remaining_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

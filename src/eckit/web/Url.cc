/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/web/Url.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/Log.h"
#include "eckit/parser/JSONParser.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"
#include "eckit/web/Html.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


inline void header(char c) {
#if 0
	if (isprint(c))
		std::cout << "header: " << c << std::endl;
	else
		std::cout << "header: " << hex << int(c) << dec << std::endl;
#endif
}

void Url::parse(const std::string& url, bool param) {


    int size  = url.length();
    int index = 0;
    std::string s, p;

    while (index < size) {

        switch (url[index]) {
            case '?':
                param = true;
                if (s != "")
                    url_.push_back(Html::removeHex(s));
                s = "";
                break;

            case '=':
                if (param) {
                    p = s;
                    s = "";
                }
                else {
                    s += url[index];
                }
                break;


            case '&':
                if (param) {
                    dict_[Html::removeHex(p)] = Html::removeHex(s);
                    s                         = "";
                    p                         = "";
                }
                else {
                    s += url[index];
                }
                break;

            case '+':
                // Do we need a multimap?
                s += url[index];
                break;

            case '/':
                if (!param) {
                    if (s != "")
                        url_.push_back(Html::removeHex(s));
                    s = "";
                }
                else
                    s += url[index];
                break;

            default:
                s += url[index];
                break;
        }

        ++index;
    }

    if (!param) {
        if (s != "")
            url_.push_back(Html::removeHex(s));
    }
    else {
        if (p != "")
            dict_[Html::removeHex(p)] = Html::removeHex(s);
    }
}

Url::Url(std::istream& in) {
    std::string url;

    in >> method_;
    in >> url;
    parse(url, false);

    char c = 0;
    while (in.get(c) && c != '\n')
        header(c);

    parse(in);
    Log::debug() << "Incomming url-> " << *this << std::endl;
}

void Url::parse(std::istream& in) {

    char c = 0;

    std::map<std::string, std::string, std::less<std::string> > m;

    for (;;) {
        std::string s;
        while (in.get(c) && c != '\r' && c != ':') {
            header(c);
            s += c;
        }

        if (c != ':') {
            header(c);
            in.get(c);  // Skip '\n'
            header(c);
            break;
        }

        header(c);
        while (in.get(c) && c == ' ')
            header(c);

        if (c != '\r') {
            header(c);

            std::string r;
            r += c;
            while (in.get(c) && c != '\r') {
                header(c);
                r += c;
            }

            m[s] = r;
            in.get(c);  // Skip '\n'
            header(c);
        }
    }

    in_ = m;

    long len = in_.contentLength();

    if (len) {
        static const std::string FormType = "application/x-www-form-urlencoded";
        // static const std::string JSONType      = "application/json";

        // bool ascii = true;
        Buffer content(len);
        const std::string& type = in_.type();


        char* p = content;

        for (int i = 0; i < len; i++) {
            in.get(c);
            header(c);
            *p++ = c;
        }

        if (type == FormType) {
            std::string s(static_cast<char*>(content), p - static_cast<char*>(content));
            parse(s, true);
        }

        in_.content(content, len);
    }

    Log::debug() << *this << std::endl;
}

Url::Url(const std::string& url) :
    method_("GET") {
    parse(url, false);
    Log::debug() << "Incomming url-> " << *this << std::endl;
}

Url::~Url() {}

std::string Url::name() const {
    std::string s = "";
    for (std::vector<std::string>::const_iterator j = url_.begin(); j != url_.end(); ++j) {
        s += "/";
        s += *j;
    }
    return s;
}

void Url::print(std::ostream& s) const {
    for (std::vector<std::string>::const_iterator j = url_.begin(); j != url_.end(); ++j)
        s << "/" << *j;

    char c = '?';
    dict_t::const_iterator i;
    for (i = dict_.begin(); i != dict_.end(); ++i) {
        s << c << (*i).first << '=' << (*i).second;
        c = '&';
    }
}

void Url::cgiParam(std::ostream& s, char sep) const {
    char c = ' ';
    dict_t::const_iterator i;
    for (i = dict_.begin(); i != dict_.end(); ++i) {
        s << c << (*i).first << '=' << (*i).second;
        c = sep;
    }
}

UrlAccess Url::operator[](const std::string& s) {
    return UrlAccess(*this, s);
}

void Url::set(const std::string& p, const std::string& s) {
    dict_[p] = s;
}

const std::string& Url::get(const std::string& s) {
    return dict_[s];
}

void Url::erase(const std::string& s) {
    dict_.erase(s);
}

const std::string& Url::operator[](int n) const {
    return url_[n];
}

eckit::Value Url::json() const {
    std::string p = in_.content();

    if (p.empty()) {
        return toValue(dict_);
    }

    std::cout << "================" << std::endl;
    std::cout << p << std::endl;
    std::cout << "================" << std::endl;

    return JSONParser::decodeString(p);
}

std::string Url::str() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

int Url::size() const {
    return url_.size();
}

HttpHeader& Url::headerIn() {
    return in_;
}

HttpHeader& Url::headerOut() {
    return out_;
}

void Url::streamFrom(DataHandle* handle, const std::string& type) {
    handle_.reset(handle);
    type_ = type;
}

DataHandle* Url::streamFrom() {
    return handle_.get();
}

const std::string& Url::streamType() const {
    return type_;
}


//----------------------------------------------------------------------------------------------------------------------

UrlAccess::operator long() {
    return Translator<std::string, long>()(url_.get(s_));
}

UrlAccess::operator std::string() {
    return url_.get(s_);
}

UrlAccess& UrlAccess::operator=(long n) {
    url_.set(s_, Translator<long, std::string>()(n));
    return *this;
}

UrlAccess& UrlAccess::operator=(const std::string& s) {
    url_.set(s_, s);
    return *this;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

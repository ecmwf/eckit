/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/utils/Regex.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Regex::Regex(const std::string& s, bool shell, bool extended) :
    str_(s), extended_(extended) {
    // Log::debug() << "Regex " << str_ << std::endl;
    if (shell) {
        long len = s.length() * 3 + 1;
        Buffer buffer(len);
        char* re = buffer;

        std::string::size_type i = 0;
        int j                    = 0;

        if (shell) {
            re[j++] = '^';
        }

        while (i < s.length()) {
            switch (s[i]) {
                case '?':
                    re[j++] = '.';
                    break;

                case '*':
                    re[j++] = '.';
                    re[j++] = '*';
                    break;

                case '.':
                    re[j++] = '\\';
                    re[j++] = '.';
                    break;

                case '[':
                    re[j++] = '[';
                    i++;
                    while (i < s.length() && s[i] != ']')
                        re[j++] = s[i++];
                    re[j++] = ']';
                    break;

                default:
                    re[j++] = s[i];
                    break;
            }
            i++;
            ASSERT(j < len);
        }
        if (shell) {
            re[j++] = '$';
        }
        re[j] = 0;
        str_  = re;
    }
    // Log::debug() << "Regex " << str_ << std::endl;
    compile(str_.c_str());
}

Regex::~Regex() {
    regfree(&re_);
}

void Regex::print(std::ostream& s) const {
    s << "/" << str_ << "/";
}

bool Regex::match(const std::string& s) const {
    regmatch_t pm;
    // Log::debug() << "Match " << s << " with " << str_ << " -> " << (regexec(&re_,s.c_str(),1,&pm,0) == 0) <<
    // std::endl;
    return regexec(&re_, s.c_str(), 1, &pm, 0) == 0;
}

void Regex::compile(const char* p) {
    int n = regcomp(&re_, p, extended_ ? REG_EXTENDED : 0);
    if (n) {
        char buf[1024];
        regerror(n, &re_, buf, sizeof(buf));
        throw SeriousBug(buf);
    }
}

Regex::Regex(const Regex& other) :
    str_(other.str_), extended_(other.extended_) {
    compile(str_.c_str());
}

Regex& Regex::operator=(const Regex& other) {
    regfree(&re_);
    str_      = other.str_;
    extended_ = other.extended_;
    compile(str_.c_str());
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

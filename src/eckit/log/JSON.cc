/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>
#include <string>

#include "eckit/log/JSON.h"
#include "eckit/types/DateTime.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static void print_indent(std::ostream& out, int indent) {
    indent = std::max(0, indent);
    out << "\n"
        << std::string(size_t(indent), ' ');
}

static bool check(const JSON::Formatting& formatting, int flag) {
    if (flag == JSON::Formatting::COMPACT) {
        return formatting.flags() == JSON::Formatting::COMPACT;
    }
    return (formatting.flags() & flag) == flag;
}

JSON::Formatting JSON::Formatting::compact() {
    return Formatting(COMPACT);
}

JSON::Formatting JSON::Formatting::indent(int indentation) {
    return Formatting(INDENT_DICT, indentation);
}

JSON::Formatting::Formatting(int formatting, int indentation) :
    flags_{formatting}, indentation_{indentation} {}

int JSON::Formatting::indentation() const {
    return indentation_;
}

int JSON::Formatting::flags() const {
    return flags_;
}

//----------------------------------------------------------------------------------------------------------------------

JSON::JSON(std::ostream& out, bool null) :
    out_(out), null_(null) {
    sep_.push_back("");
    state_.push_back(true);
}

JSON::JSON(std::ostream& out, JSON::Formatting formatting) :
    JSON(out, true) {
    formatting_ = formatting;
}

JSON::~JSON() {
    if (null_)
        out_ << "null";
}

void JSON::sep() {
    null_ = false;
    out_ << sep_.back();
    if (sep_.back() == ",") {
        bool indent = false;
        if (check(formatting_, Formatting::INDENT_DICT) && indict()) {
            indent = true;
        }
        if (check(formatting_, Formatting::INDENT_LIST) && inlist()) {
            indent = true;
        }
        if (indent) {
            print_indent(out_, indentation_);
        }
    }
    std::string colon = check(formatting_, Formatting::COMPACT) ? ":" : " : ";
    if (indict() && sep_.back() != colon) {
        sep_.back() = colon;
    }
    else {
        sep_.back() = ",";
    }
}

static std::ostream& encode(std::ostream& s, const char* p) {
    s << '"';
    while (*p) {
        switch (*p) {

            case '\\':
                s << "\\\\";
                break;

            case '\n':
                s << "\\n";
                break;

            case '\t':
                s << "\\t";
                break;

            case '\b':
                s << "\\b";
                break;

            case '\f':
                s << "\\f";
                break;

            case '\r':
                s << "\\r";
                break;

            case '"':
                s << "\\\"";
                break;

            default:
                s << *p;
                break;
        }
        p++;
    }

    s << '"';
    return s;
}


JSON& JSON::startObject() {
    null_ = false;
    sep();
    sep_.push_back("");
    state_.push_back(true);
    out_ << "{";
    if (check(formatting_, Formatting::INDENT_DICT)) {
        indentation_ += formatting_.indentation();
        print_indent(out_, indentation_);
    }
    return *this;
}

JSON& JSON::null() {
    null_ = false;
    sep();
    out_ << "null";
    return *this;
}

JSON& JSON::startList() {
    null_ = false;
    sep();
    sep_.push_back("");
    state_.push_back(false);
    out_ << "[";
    if (check(formatting_, Formatting::INDENT_LIST)) {
        indentation_ += formatting_.indentation();
        print_indent(out_, indentation_);
    }
    return *this;
}

JSON& JSON::endObject() {
    sep_.pop_back();
    state_.pop_back();
    if (check(formatting_, Formatting::INDENT_DICT)) {
        indentation_ -= formatting_.indentation();
        print_indent(out_, indentation_);
    }
    out_ << "}";
    return *this;
}

JSON& JSON::endList() {
    sep_.pop_back();
    state_.pop_back();
    if (check(formatting_, Formatting::INDENT_LIST)) {
        indentation_ -= formatting_.indentation();
        print_indent(out_, indentation_);
    }
    out_ << "]";
    return *this;
}

JSON& JSON::operator<<(bool n) {
    null_ = false;
    sep();
    out_ << (n ? "true" : "false");
    return *this;
}

JSON& JSON::operator<<(char n) {
    null_ = false;
    sep();
    out_ << '"' << n << '"';
    return *this;
}

JSON& JSON::operator<<(unsigned char n) {
    null_ = false;
    sep();
    out_ << '"' << n << '"';
    return *this;
}

JSON& JSON::operator<<(int n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(unsigned int n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(long n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(unsigned long n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(long long n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(unsigned long long n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(float n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(double n) {
    null_ = false;
    sep();
    out_ << n;
    return *this;
}

JSON& JSON::operator<<(const std::string& s) {
    null_ = false;
    sep();
    encode(out_, s.c_str());
    return *this;
}

JSON& JSON::operator<<(const char* s) {
    null_ = false;
    sep();
    encode(out_, s);
    return *this;
}

JSON& JSON::operator<<(const Date& date) {
    *this << std::string(date);
    return *this;
}

JSON& JSON::operator<<(const Time& time) {
    *this << std::string(time);
    return *this;
}

JSON& JSON::operator<<(const DateTime& datetime) {
    *this << datetime.iso(false);
    return *this;
}

JSON& JSON::operator<<(const ::timeval& t) {
    *this << double(t.tv_sec + t.tv_usec / 1000000.0);
    return *this;
}

JSON& JSON::precision(int n) {
    out_ << std::setprecision(n);
    return *this;
}

void JSON::raw(const char* buffer, long len) {
    out_ << std::string(buffer, size_t(len));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

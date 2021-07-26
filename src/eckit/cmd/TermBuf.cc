/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <iomanip>
#include <iostream>

#include "eckit/cmd/TermBuf.h"

extern "C" {
int TermPutChar(int);
void TermInit();
void TermClear();
void TermHome();
void TermClearEOL();
void TermClearEOS();
}

static std::ostream* os = 0;

int TermPutChar(int c) {
    *os << char(c);
    return c;
}

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TermBuf::TermBuf(std::ostream& o) :
    out_(o) {
    setp(buffer_, buffer_ + sizeof(buffer_));
}

TermBuf::~TermBuf() {
    sync();
}

int TermBuf::sync() {
    for (char* c = pbase(); c != pptr(); ++c) {
        if (*c == '\n')
            clearEOL();
        out_ << *c;
    }
    setp(pbase(), epptr());
    out_ << std::flush;
    return 0;
}

int TermBuf::overflow(int c) {
    sync();
    if (c == EOF)
        return 0;

    sputc(c);
    return 0;
}

void TermBuf::init() {
    os = &out_;
    TermInit();
}

void TermBuf::clear() {
    os = &out_;
    TermClear();
}

void TermBuf::home() {
    os = &out_;
    TermHome();
}

void TermBuf::clearEOL() {
    os = &out_;
    TermClearEOL();
}

void TermBuf::clearEOS() {
    os = &out_;
    TermClearEOS();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

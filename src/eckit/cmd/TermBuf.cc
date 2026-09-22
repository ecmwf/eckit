// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/TermBuf.h"

#include <iomanip>
#include <iostream>

extern "C" {
int TermPutChar(int);
void TermInit();
void TermClear();
void TermHome();
void TermClearEOL();
void TermClearEOS();
}

static std::ostream* os = nullptr;

int TermPutChar(int c) {
    *os << char(c);
    return c;
}

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

TermBuf::TermBuf(std::ostream& o) : out_(o) {
    setp(buffer_, buffer_ + sizeof(buffer_));
}

TermBuf::~TermBuf() {
    sync();
}

int TermBuf::sync() {
    for (char* c = pbase(); c != pptr(); ++c) {
        if (*c == '\n') {
            clearEOL();
        }
        out_ << *c;
    }
    setp(pbase(), epptr());
    out_ << std::flush;
    return 0;
}

int TermBuf::overflow(int c) {
    sync();
    if (c == EOF) {
        return 0;
    }

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

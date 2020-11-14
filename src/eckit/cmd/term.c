/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <curses.h>
#include <stdio.h>
#include <term.h>

/* Term should a class, but curses.h is not C++ compatible */

extern int TermPutChar(int);

void TermInit() {
    setupterm((char*)0, 1, (int*)0);
}

void TermHome() {
    tputs(cursor_home, 1, TermPutChar);
}

void TermClearEOL() {
    tputs(clr_eol, 1, TermPutChar);
}

void TermClearEOS() {
    tputs(clr_eos, 1, TermPutChar);
}

void TermClear() {
    tputs(clear_screen, 1, TermPutChar);
}

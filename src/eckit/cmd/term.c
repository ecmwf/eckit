// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

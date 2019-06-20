/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <errno.h>
#include <unistd.h>

#include <iostream>

#include "eckit/config/Resource.h"
#include "eckit/log/Colour.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static int xindex = std::ios::xalloc();

enum
{
    RESET     = 0,
    BOLD      = 1,
    UNDERLINE = 4,
    BLINK     = 5,
    REVERSE   = 7,
    HIDDEN    = 8
};
enum
{
    BLACK   = 0,
    RED     = 1,
    GREEN   = 2,
    YELLOW  = 3,
    BLUE    = 4,
    MAGENTA = 5,
    CYAN    = 6,
    WHITE   = 7
};

//#define X(a) putchar(a)
//#define X(a) out << (a)
#define X(a) out << char(a)

static bool connected_to_console() {
    int save_errno = errno;
    bool result    = ::isatty(1);  // This may change errno
    errno          = save_errno;
    return result;
}

static std::ostream& put(std::ostream& out, int fg, int bg, int attr) {
    static bool colourOutput = Resource<bool>("$ECKIT_COLOUR_OUTPUT;-colour;colourOutput", connected_to_console());

    if (colourOutput && out.iword(xindex) == 0) {
        int n = 0;
        X(char(0x1B));
        X('[');

        if (attr >= 0) {
            n++;
            X(attr + '0');
        }

        if (fg >= 0) {
            if (n) {
                X(';');
            }
            X('3');
            X(fg + '0');
            n++;
        }
        if (bg >= 0) {
            if (n) {
                X(';');
            }
            X('4');
            X(bg + '0');
            n++;
        }

        X('m');
    }

    return out;
}


std::ostream& Colour::on(std::ostream& s) {
    s.iword(xindex) = 0;
    return s;
}

std::ostream& Colour::off(std::ostream& s) {
    s.iword(xindex) = 1;
    return s;
}

std::ostream& Colour::reset(std::ostream& s) {
    return put(s, -1, -1, RESET);
}

std::ostream& Colour::bold(std::ostream& s) {
    return put(s, -1, -1, BOLD);
}

std::ostream& Colour::underline(std::ostream& s) {
    return put(s, -1, -1, UNDERLINE);
}

std::ostream& Colour::blink(std::ostream& s) {
    return put(s, -1, -1, BLINK);
}

std::ostream& Colour::reverse(std::ostream& s) {
    return put(s, -1, -1, REVERSE);
}

std::ostream& Colour::hidden(std::ostream& s) {
    return put(s, -1, -1, HIDDEN);
}

std::ostream& Colour::black(std::ostream& s) {
    return put(s, BLACK, -1, -1);
}

std::ostream& Colour::red(std::ostream& s) {
    return put(s, RED, -1, -1);
}

std::ostream& Colour::green(std::ostream& s) {
    return put(s, GREEN, -1, -1);
}

std::ostream& Colour::yellow(std::ostream& s) {
    return put(s, YELLOW, -1, -1);
}

std::ostream& Colour::magenta(std::ostream& s) {
    return put(s, MAGENTA, -1, -1);
}

std::ostream& Colour::blue(std::ostream& s) {
    return put(s, BLUE, -1, -1);
}

std::ostream& Colour::cyan(std::ostream& s) {
    return put(s, CYAN, -1, -1);
}

std::ostream& Colour::white(std::ostream& s) {
    return put(s, WHITE, -1, -1);
}

std::ostream& Colour::blackBackground(std::ostream& s) {
    return put(s, -1, BLACK, -1);
}
std::ostream& Colour::redBackground(std::ostream& s) {
    return put(s, -1, RED, -1);
}
std::ostream& Colour::greenBackground(std::ostream& s) {
    return put(s, -1, GREEN, -1);
}

std::ostream& Colour::yellowBackground(std::ostream& s) {
    return put(s, -1, YELLOW, -1);
}


std::ostream& Colour::magentaBackground(std::ostream& s) {
    return put(s, -1, MAGENTA, -1);
}


std::ostream& Colour::blueBackground(std::ostream& s) {
    return put(s, -1, BLUE, -1);
}

std::ostream& Colour::cyanBackground(std::ostream& s) {
    return put(s, -1, CYAN, -1);
}

std::ostream& Colour::whiteBackground(std::ostream& s) {
    return put(s, -1, WHITE, -1);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

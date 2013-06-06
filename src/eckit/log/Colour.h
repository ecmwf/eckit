/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Colour_h
#define eckit_Colour_h

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Colour {
public:

    static ostream& on(ostream&);
    static ostream& off(ostream&);

    static ostream& reset(ostream&);

    static ostream& bold(ostream&);
    static ostream& underline(ostream&);
    static ostream& blink(ostream&);
    static ostream& reverse(ostream&);
    static ostream& hidden(ostream&);

    static ostream& black(ostream&);
    static ostream& red(ostream&);
    static ostream& green(ostream&);
    static ostream& yellow(ostream&);
    static ostream& magenta(ostream&);
    static ostream& blue(ostream&);
    static ostream& cyan(ostream&);
    static ostream& white(ostream&);

    static ostream& blackBackground(ostream&);
    static ostream& redBackground(ostream&);
    static ostream& greenBackground(ostream&);
    static ostream& yellowBackground(ostream&);
    static ostream& magentaBackground(ostream&);
    static ostream& blueBackground(ostream&);
    static ostream& cyanBackground(ostream&);
    static ostream& whiteBackground(ostream&);

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

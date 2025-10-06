/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Colour.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Nov 2011

#ifndef eckit_Colour_h
#define eckit_Colour_h

#include <iosfwd>

#include "eckit/eckit.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Colour {
public:

    static std::ostream& on(std::ostream&);
    static std::ostream& off(std::ostream&);

    static std::ostream& reset(std::ostream&);

    static std::ostream& bold(std::ostream&);
    static std::ostream& underline(std::ostream&);
    static std::ostream& blink(std::ostream&);
    static std::ostream& reverse(std::ostream&);
    static std::ostream& hidden(std::ostream&);

    static std::ostream& black(std::ostream&);
    static std::ostream& red(std::ostream&);
    static std::ostream& green(std::ostream&);
    static std::ostream& yellow(std::ostream&);
    static std::ostream& magenta(std::ostream&);
    static std::ostream& blue(std::ostream&);
    static std::ostream& cyan(std::ostream&);
    static std::ostream& white(std::ostream&);

    static std::ostream& blackBackground(std::ostream&);
    static std::ostream& redBackground(std::ostream&);
    static std::ostream& greenBackground(std::ostream&);
    static std::ostream& yellowBackground(std::ostream&);
    static std::ostream& magentaBackground(std::ostream&);
    static std::ostream& blueBackground(std::ostream&);
    static std::ostream& cyanBackground(std::ostream&);
    static std::ostream& whiteBackground(std::ostream&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

#ifndef Colour_H
#define Colour_H

#include "eclib/machine.h"

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


#endif

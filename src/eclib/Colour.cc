#include "eclib/Colour.h"
#include "eclib/Resource.h"

static int xindex = ios::xalloc();

enum { RESET = 0, BOLD  =  1,  UNDERLINE  =  4, BLINK =  5, REVERSE = 7, HIDDEN =8 } ;
enum { BLACK = 0, RED     =  1, GREEN = 2,  YELLOW    = 3, BLUE = 4, MAGENTA = 5, CYAN  = 6, WHITE = 7};

//#define X(a) putchar(a)
//#define X(a) out << (a)
#define X(a) out << char(a)

static ostream& put(ostream& out, int fg, int bg, int attr)
{
    static bool colourOutput = Resource<bool>("$ECLIB_COLOUR_OUTPUT;-colour;colourOutput", false);
    
	if (colourOutput && out.iword(xindex) == 0)
    {
        int n = 0;
        X( char(0x1B) );
        X( '[');

        if (attr>=0)
        {
            n++;
            X( attr + '0');
        }

        if (fg >= 0) {
            if (n) {
                X( ';');
            }
            X( '3');
            X( fg + '0');
            n++;
        }
        if (bg >= 0) {
            if (n) {
                X( ';');
            }
            X( '4');
            X( bg + '0');
            n++;
        }

        X( 'm');

    }

    return out;
}



ostream& Colour::on(ostream& s) {
    s.iword(xindex) = 0;
    return s;
}

ostream& Colour::off(ostream& s) {
    s.iword(xindex) = 1;
    return s;
}

ostream& Colour::reset(ostream& s) {
    return put(s,-1,-1,RESET);
}

ostream& Colour::bold(ostream& s) {
    return put(s,-1,-1,BOLD);
}

ostream& Colour::underline(ostream& s) {
    return put(s,-1,-1,UNDERLINE);
}

ostream& Colour::blink(ostream& s) {
    return put(s,-1,-1,BLINK);
}

ostream& Colour::reverse(ostream& s) {
    return put(s,-1,-1,REVERSE);
}

ostream& Colour::hidden(ostream& s) {
    return put(s,-1,-1,HIDDEN);
}

ostream& Colour::black(ostream& s) {
    return put(s,BLACK,-1,-1);
}

ostream& Colour::red(ostream& s) {
    return put(s,RED,-1,-1);
}

ostream& Colour::green(ostream& s) {
    return put(s,GREEN,-1,-1);
}

ostream& Colour::yellow(ostream& s) {
    return put(s,YELLOW,-1,-1);
}

ostream& Colour::magenta(ostream& s) {
    return put(s,MAGENTA,-1,-1);
}

ostream& Colour::blue(ostream& s) {
    return put(s,BLUE,-1,-1);
}

ostream& Colour::cyan(ostream& s) {
    return put(s,CYAN,-1,-1);
}

ostream& Colour::white(ostream& s) {
    return put(s,WHITE,-1,-1);
}

ostream& Colour::blackBackground(ostream& s) {
    return put(s,-1,BLACK,-1);
}
ostream& Colour::redBackground(ostream& s) {
    return put(s,-1,RED,-1);
}
ostream& Colour::greenBackground(ostream& s) {
    return put(s,-1,GREEN,-1);
}

ostream& Colour::yellowBackground(ostream& s) {
    return put(s,-1,YELLOW,-1);
}


ostream& Colour::magentaBackground(ostream& s) {
    return put(s,-1,MAGENTA,-1);
}


ostream& Colour::blueBackground(ostream& s) {
    return put(s,-1,BLUE,-1);
}

ostream& Colour::cyanBackground(ostream& s) {
    return put(s,-1,CYAN,-1);
}

ostream& Colour::whiteBackground(ostream& s) {
    return put(s,-1,WHITE,-1);
}

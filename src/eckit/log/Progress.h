// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Progress.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eckit_Progress_h
#define eckit_Progress_h

#include <string>


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Progress {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Progress(const std::string&, unsigned long long, unsigned long long);

    // -- Destructor

    ~Progress();

    // -- Convertors
    // None

    // -- Operators

    void operator()(unsigned long long);

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None


protected:

    // -- Members
    // None

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    Progress(const Progress&);
    Progress& operator=(const Progress&);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const Progress& p)
    //	{ p.print(s); return s; }
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

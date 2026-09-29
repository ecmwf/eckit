// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File JavaResource.h
// Baudouin Raoult - ECMWF Sep 97

#ifndef JavaResource_H
#define JavaResource_H

#include "eckit/web/HtmlResource.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class JavaResource : public HtmlResource {
public:

    // -- Exceptions
    // None

    // -- Contructors

    JavaResource();

    // -- Destructor

    ~JavaResource() override;

    // -- Convertors
    // None

    // -- Operators
    // None

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

    std::string name_;

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods

    void GET(std::ostream&, Url&) override;

    // -- Class members
    // None

    // -- Class methods


private:

    // No copy allowed

    JavaResource(const JavaResource&);
    JavaResource& operator=(const JavaResource&);

    // -- Members


    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const JavaResource& p)
    //	{ p.print(s); return s; }
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif

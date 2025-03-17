/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

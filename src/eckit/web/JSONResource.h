/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HtmlResource.h
// Baudouin Raoult - ECMWF Jun 12

#ifndef JSONResource_H
#define JSONResource_H

#include "eckit/web/HtmlResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class JSON;
class Value;

class JSONResource : public HtmlResource {
public:

// -- Contructors

    JSONResource(const std::string&);

// -- Destructor

    virtual ~JSONResource();

private:

    virtual void html(std::ostream&,Url&);
    virtual void json(eckit::JSON&,const eckit::Value&) = 0;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

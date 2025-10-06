/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date Jun 2012

#ifndef eckit_web_JSONResource_H
#define eckit_web_JSONResource_H

#include "eckit/web/HttpResource.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class JSON;
class Value;

class JSONResource : public HttpResource {
public:

    JSONResource(const std::string&);

    ~JSONResource() override;

private:

    void GET(std::ostream&, Url&) override;
    void POST(std::ostream&, Url&) override;
    virtual void json(eckit::JSON&, const eckit::Value&) = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

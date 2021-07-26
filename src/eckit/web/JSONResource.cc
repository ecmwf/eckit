/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/web/JSONResource.h"
#include "eckit/log/JSON.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONResource::JSONResource(const std::string& name) :
    HttpResource(name) {}

JSONResource::~JSONResource() {}

void JSONResource::GET(std::ostream& out, Url& url) {
    url.headerOut().type("application/json");
    JSON j(out, false);
    json(j, url.json());
}

void JSONResource::POST(std::ostream& out, Url& url) {
    url.headerOut().type("application/json");
    JSON j(out, false);
    json(j, url.json());
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/JSONResource.h"

#include "eckit/log/JSON.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONResource::JSONResource(const std::string& name) : HttpResource(name) {}

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

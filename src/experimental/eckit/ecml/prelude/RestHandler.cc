/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "RestHandler.h"

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"

namespace eckit {

RestHandler::RestHandler(const std::string& name) : RequestHandler(name) {}

Values RestHandler::handle(ExecutionContext& context)
{
    Values vs (context.environment().lookup("of"));

    if (! vs->rest())
        return new Cell("_list", "", 0, 0);

    return Cell::clone(vs->rest());
}

} // namespace eckit

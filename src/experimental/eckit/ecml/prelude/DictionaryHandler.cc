/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "DictionaryHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"

namespace eckit {

DictionaryHandler::DictionaryHandler(const std::string& name) : RequestHandler(name) {}

Values DictionaryHandler::handle(ExecutionContext& context)
{
    Cell * frame (context.environment().currentFrame());
    return new Cell("_list", "", Cell::clone(frame), 0);
}

} // namespace eckit

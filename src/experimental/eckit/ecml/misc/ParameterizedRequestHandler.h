/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, June 2015

#ifndef eckit_utils_ParameterizedRequestHandler_H
#define eckit_utils_ParameterizedRequestHandler_H

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/RequestHandler.h"

#include "eckit/ecml/misc/DynamicParametrisation.h"

#include <string>

namespace eckit { 

class ExecutionContext;
class MultiHandle; 

class ParameterizedRequestHandler : public eckit::RequestHandler {
public:
    ParameterizedRequestHandler(const std::string&);
    ~ParameterizedRequestHandler();

    virtual Values handle(eckit::ExecutionContext&);

    virtual std::vector<std::string> handle(eckit::Parametrisation&) = 0;
};

} //namespace eckit 

#endif

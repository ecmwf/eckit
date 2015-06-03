/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2015

#ifndef eckit_utils_Interpreter_H
#define eckit_utils_Interpreter_H

#include "eckit/parser/Request.h"
#include <list>

namespace eckit {

class ExecutionContext;

class Interpreter {
public:
    virtual Values eval(const Request, ExecutionContext&);

protected:
    virtual Values evalList(const Request, ExecutionContext&);
    virtual Values evalRequests(const Request, ExecutionContext&);
    virtual Values evalLet(const Request, ExecutionContext&);
    virtual Values defineFunction(const Request, ExecutionContext&);
    virtual Values evalFunction(const Request, const Request, ExecutionContext&);
    virtual Values evalNative(const Request object, const Request request, ExecutionContext&);
    virtual Request evalAttributes(const Request, ExecutionContext&);
};

} // namespace eckit

#endif

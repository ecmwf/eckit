/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include <list>

#include "eckit/ecml/parser/Request.h"

namespace eckit {

class ExecutionContext;

class Interpreter {
public:
    Interpreter();
    virtual ~Interpreter();

    virtual Values eval(const Request, ExecutionContext&);
    virtual Request evalAttributes(const Request, ExecutionContext&);

    void debug(bool d) { debug_ = d; }

    virtual Values evalRequests(const Request, ExecutionContext&);
    virtual Values evalClosure(const Request object, const Request request, ExecutionContext&);
    virtual Values evalNative(const Request object, const Request request, ExecutionContext&);
protected:
    virtual Values evalList(const Request, ExecutionContext&);
    virtual Request evalMacro(const Request, const Request, ExecutionContext&);

private:
    bool debug_;
};

} // namespace eckit

#endif

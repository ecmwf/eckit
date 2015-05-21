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

#ifndef Interpreter_H
#define Interpreter_H

#include "eckit/parser/Request.h"
#include <list>

class ExecutionContext;

class Interpreter {
public:
    Values eval(const Request, ExecutionContext&);
private:
    Values evalList(const Request, ExecutionContext&);
};

#endif

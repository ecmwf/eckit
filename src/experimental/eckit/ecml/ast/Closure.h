/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef eckit_ecml_Closure_H
#define eckit_ecml_Closure_H

#include <vector>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "experimental/eckit/ecml/parser/Request.h"

namespace eckit {

class FunctionDefinition;
class ExecutionContext;

// E.g.: closure,name=f,parameters=x,captured=(environment,a=1,b=2),code=(println,values=FOO)
class Closure {
public:
    Closure(Cell*);
    Closure(const FunctionDefinition&, ExecutionContext&);
    ~Closure();

    Cell* capturedEnvironment() const; 
    Cell* code() const;

    operator Cell*() const;

private:
    std::string name_;
    std::vector<std::string> parameters_;
    Cell* capturedEnvironment_;
    Cell* code_;
};

} // namespace eckit


#endif

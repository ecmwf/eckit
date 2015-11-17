/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef eckit_ecml_FunctionDefinition_H
#define eckit_ecml_FunctionDefinition_H

#include <vector>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/ecml/parser/Request.h"

namespace eckit {

class FunctionDefinition {
public:
    FunctionDefinition(Cell*);
    std::string name() const;
    std::vector<std::string> parameters() const;
    std::vector<std::string> capturedVariables() const;
    Cell* code() const;

private:
    std::string name_;
    std::vector<std::string> parameters_;
    std::vector<std::string> capturedVariables_;
    Cell* code_;
};

} // namespace eckit


#endif

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

#ifndef eckit_utils_SpecialFormHandler_H
#define eckit_utils_SpecialFormHandler_H

#include "eckit/filesystem/PathName.h"
#include "parser/Request.h"

namespace eckit { 

class ExecutionContext;

class SpecialFormHandler {
public:
    virtual Request handle(const Request, ExecutionContext&) = 0;

    virtual std::string name() const;

    static SpecialFormHandler& handler(const std::string&);

protected:
    SpecialFormHandler(const std::string&);

    std::string name_;
    
    static std::map<std::string,SpecialFormHandler*> registeredHandlers_;
};

} //namespace eckit 

#endif

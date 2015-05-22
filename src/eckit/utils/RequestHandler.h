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

#ifndef RequestHandler_H
#define RequestHandler_H

#include "eckit/filesystem/PathName.h"
#include "eckit/parser/Request.h"

class ExecutionContext;
namespace eckit { class MultiHandle; }

class RequestHandler {
public:
    virtual Values handle(const Request) = 0;
    virtual Values handle(const Request, ExecutionContext&) = 0;

    virtual std::string name() const;

    static std::string database(const Request request);
    static long port(const Request request);

protected:
    RequestHandler(const std::string&);

    static std::vector<std::string> getValueAsList(const Request request, const std::string& keyword);
    static void popIfNotSet(const std::string&, Request, ExecutionContext&);
    static std::vector<std::string> pathNamesToStrings(const std::vector<eckit::PathName>&);

    std::string name_;
};

#endif

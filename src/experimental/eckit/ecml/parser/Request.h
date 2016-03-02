/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta - ECMWF March 2015

#ifndef eckit_parser_Request_H
#define eckit_parser_Request_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ostream>
#include <iostream> 
#include <sstream>

#include "eckit/ecml/parser/Cell.h"
#include "eckit/ecml/parser/List.h"

namespace eckit {

typedef Cell* Values;
typedef Cell* Request;

template <typename MARSREQUEST>
static void convertToMarsRequest(const Request request, MARSREQUEST& marsRequest)
{
    //marsRequest.name(verb(request));
    marsRequest.name(request->text());
    std::cerr << "convertToMarsRequest: verb is " << request->text() << std::endl;
    for (eckit::Request r(request->rest()); r; r = r->rest())
    {
        std::string key (r->text());
        std::vector<std::string> vs;
        for (eckit::Request v (r->value()); v; v = v->rest())
        {
            std::stringstream ss;
            if (v->value()) 
                ss << v->value(); 
            vs.push_back(ss.str());
        }
        marsRequest.setValues(key, vs);
    }
}

} // namespace eckit

#endif

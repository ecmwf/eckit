/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File RequestParser.h
// Manuel Fuentes - ECMWF Jan 97
// Piotr Kuchta - ECMWF April 2015

#ifndef RequestParser_H
#define RequestParser_H

#include "eckit/parser/Request.h"

namespace eckit {

class RequestParser
{
public:
    static Request parseFile(const char*, bool debug=false);
    static Request parse(const std::string&, bool debug=false);
    static Request parseFileUnprotected(const char*, bool debug=false);

private:
    RequestParser(const RequestParser&);
    RequestParser& operator=(const RequestParser&);
};

} // namespace eckit

#endif

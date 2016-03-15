/*
 * (C) Copyright 1996-2016 ECMWF.
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

#ifndef eckit_parser_RequestParser_H
#define eckit_parser_RequestParser_H

#include "eckit/ecml/parser/Request.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

class RequestParser : private eckit::NonCopyable
{
public:
    static Request parse(const std::string&, bool debug=false);
    static Request parseFile(const char*, bool debug=false);

    static std::string readFile(const PathName& fileName, bool logging);
};

} // namespace eckit

#endif

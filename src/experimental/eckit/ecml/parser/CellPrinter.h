/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta - ECMWF March 2015

#ifndef eckit_parser_CellPrinter_H
#define eckit_parser_CellPrinter_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ostream>

namespace eckit {

class Cell;

class CellPrinter {
public:
    static std::ostream& print(std::ostream&, const Cell*, size_t depth=0);
    static std::ostream& printAttributes(std::ostream&, const Cell*, size_t depth=0);
    static std::ostream& printValues(std::ostream&, const Cell*, size_t depth=0);

    static std::string quotedSnippet(const std::string&);
    static std::string quote(const std::string&);
    static std::vector<std::string> quote(const std::vector<std::string>&);
    static std::string snippet(const std::string& s, size_t maxCharacters, size_t maxLines);
};

} // namespace eckit

#endif

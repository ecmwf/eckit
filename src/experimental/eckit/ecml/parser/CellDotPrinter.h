/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta - ECMWF March 2015

#ifndef eckit_parser_CellDotPrinter_H
#define eckit_parser_CellDotPrinter_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ostream>

namespace eckit {

class CellDotPrinter {
public:
    static std::ostream& dot(std::ostream&, const Cell*, const std::string&, bool, bool);
    static std::ostream& printDot(std::ostream&, const Cell*, bool, bool);
    static std::ostream& printDotList(std::ostream&, const Cell*, bool);
    static std::ostream& printDotVerb(std::ostream&, const Cell*, bool);
};

} // namespace eckit

#endif

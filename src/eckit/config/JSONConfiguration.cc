/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Jul 2015

#include "eckit/config/JSONConfiguration.h"


// #include <iostream>
// #include <limits>
// #include "eckit/filesystem/PathName.h"
#include "eckit/parser/JSONParser.h"
// #include "mir/util/Parser.h"
// #include "eckit/parser/Tokenizer.h"


namespace eckit {

static Value root(std::istream &in) {
    ASSERT(in);
    eckit::JSONParser parser(in);
    Value root = parser.parse();
    return root;
}


static Value root(const std::string &path) {
    std::ifstream in(path.c_str());
    if(!in)
        throw eckit::CantOpenFile(path);
    return root(in);
}


static Value root(Stream& in) {
    std::string val;
    in.next(val);
    std::istringstream iss(val);
    return root(iss);
}


JSONConfiguration::JSONConfiguration(const eckit::PathName &path, char separator):
    Configuration(root(path), separator),
    path_(path) {
}

JSONConfiguration::JSONConfiguration(std::istream &in, char separator):
    Configuration(root(in), separator),
    path_("<istream>") {
}

JSONConfiguration::JSONConfiguration(Stream& in, char separator)
    : Configuration(root(in), separator),
      path_("<Stream>") {}

JSONConfiguration::~JSONConfiguration() {
}

void JSONConfiguration::print(std::ostream &out) const {
    out << "JSONConfiguration[path=" << path_ << ", root=" << root_ << "]";
}

} // namespace eckit


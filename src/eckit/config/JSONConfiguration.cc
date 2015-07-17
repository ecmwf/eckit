/*
 * (C) Copyright 1996-2015 ECMWF.
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
    eckit::JSONParser parser(in);
    Value root = parser.parse();
    ASSERT(!in.bad());
    return root;
}


static Value root(const eckit::PathName &path) {
    std::ifstream in(path);
    return root(in);
}


JSONConfiguration::JSONConfiguration(const eckit::PathName &path, char separator):
    Configuration(root(path), separator) {
}

JSONConfiguration::JSONConfiguration(std::istream &in, char separator):
    Configuration(root(in), separator) {
}

JSONConfiguration::~JSONConfiguration(){
}

void JSONConfiguration::print(std::ostream &out) const {
    out << "JSONConfiguration[root=";
    out << root_;
    out << "]";
}

void JSONConfiguration::set(const std::string &name, const std::string &value) {
    NOTIMP;
}

void JSONConfiguration::set(const std::string &name, const char *value) {
    NOTIMP;
}

void JSONConfiguration::set(const std::string &name, double value) {
    NOTIMP;
}

void JSONConfiguration::set(const std::string &name, long value) {
    NOTIMP;
}

void JSONConfiguration::set(const std::string &name, bool value) {
    NOTIMP;
}


}  // namespace mir


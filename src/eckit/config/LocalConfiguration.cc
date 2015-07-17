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

#include "eckit/config/LocalConfiguration.h"


// #include <iostream>
// #include <limits>
// #include "eckit/filesystem/PathName.h"
#include "eckit/parser/JSONParser.h"
// #include "mir/util/Parser.h"
#include "eckit/parser/Tokenizer.h"


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


LocalConfiguration::LocalConfiguration(const Configuration &other, const std::string &path):
    Configuration(other.lookUp(path), other.separator()) {
}

LocalConfiguration::~LocalConfiguration(){
}

void LocalConfiguration::print(std::ostream &out) const {
    out << "LocalConfiguration[root=";
    out << root_;
    out << "]";
}

void LocalConfiguration::set(const std::vector<std::string> &path, size_t i, eckit::Value &root, const eckit::Value &value)  {
    if (root.shared()) {
        // std::cout << "Clone " << root << std::endl;
        root = root.clone();
    }

    if (i + 1 == path.size()) {
        // std::cout << i << " SET " << path[i] << " to " << value << std::endl;
        root[path[i]] = value;
        return;
    }

    if (!root.contains(path[i])) {
        // std::cout << i << " NEW " << path[i]  << std::endl;
        root[path[i]] = eckit::Value::makeMap();
    }

    eckit::Value &r = root.element(path[i]);
    set(path, i + 1, r, value);
}

void LocalConfiguration::set(const std::string &s, const eckit::Value &value)  {

    // std::cout << "---- " << s << " => " << value << std::endl;

    eckit::Tokenizer parse(separator_);
    std::vector<std::string> path;
    parse(s, path);

    set(path, 0, root_, value);
}

void LocalConfiguration::set(const std::string &s, long value)  {
    set(s, eckit::Value(value));
}


void LocalConfiguration::set(const std::string &s, const char *value)  {
    set(s, eckit::Value(value));
}


void LocalConfiguration::set(const std::string &s, const std::string &value)  {
    set(s, eckit::Value(value));
}

void LocalConfiguration::set(const std::string &s, double value)  {
    set(s, eckit::Value(value));
}

void LocalConfiguration::set(const std::string &s, bool value)  {
    set(s, eckit::Value(value));
}

}  // namespace mir


/*
 * (C) Copyright 1996-2017 ECMWF.
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
#include "eckit/parser/JSONParser.h"
#include "eckit/parser/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LocalConfiguration::LocalConfiguration(char separator):
    Configuration(Value::makeMap(), separator) {
}

LocalConfiguration::LocalConfiguration(const Value& root, char separator):
    Configuration(root, separator) {
}

LocalConfiguration::LocalConfiguration(const Configuration &other):
    Configuration(other) {
}

LocalConfiguration::LocalConfiguration(const Configuration &other, const std::string &path):
    Configuration(other, path) {
}

LocalConfiguration::~LocalConfiguration() {
}

void LocalConfiguration::print(std::ostream &out) const {
    out << "LocalConfiguration[root=";
    out << root_;
    out << "]";
}

void LocalConfiguration::setValue(const std::vector<std::string> &path, size_t i, eckit::Value &root, const eckit::Value &value)  {
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
    setValue(path, i + 1, r, value);
}

void LocalConfiguration::setValue(const std::string &s, const eckit::Value &value) {

    // std::cout << "---- " << s << " => " << value << std::endl;

    eckit::Tokenizer parse(separator_);
    std::vector<std::string> path;
    parse(s, path);

    setValue(path, 0, root_, value);
}

LocalConfiguration& LocalConfiguration::set(const std::string &s, long value)  {
    setValue(s, eckit::Value(value));
    return *this;
}


LocalConfiguration& LocalConfiguration::set(const std::string &s, const char *value)  {
    setValue(s, eckit::Value(value));
    return *this;
}


LocalConfiguration& LocalConfiguration::set(const std::string &s, const std::string &value)  {
    setValue(s, eckit::Value(value));
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string &s, double value)  {
    setValue(s, eckit::Value(value));
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string &s, bool value)  {
    setValue(s, eckit::Value(value));
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string &s, size_t value)  {
    setValue(s, eckit::Value(value));
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string& s, const std::vector<long>& value) {
    ValueList values;
    for (std::vector<long>::const_iterator v = value.begin(); v != value.end(); ++v) {
        values.push_back(eckit::Value(*v));
    }
    setValue(s, values);
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string& s, const std::vector<double>& value) {
    ValueList values;
    for (std::vector<double>::const_iterator v = value.begin(); v != value.end(); ++v) {
        values.push_back(eckit::Value(*v));
    }
    setValue(s, values);
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string &s, const LocalConfiguration& value)  {
    setValue(s, value.root_);
    return *this;
}

LocalConfiguration& LocalConfiguration::set(const std::string& s, const std::vector<LocalConfiguration>& value) {
    ValueList values;
    for (std::vector<LocalConfiguration>::const_iterator v = value.begin(); v != value.end(); ++v) {
        values.push_back(v->root_);
    }
    setValue(s, values);
    return *this;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit


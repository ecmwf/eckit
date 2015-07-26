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

#include "eckit/config/Configuration.h"
#include "eckit/parser/Tokenizer.h"
#include "LocalConfiguration.h"

namespace eckit {


Configuration::Configuration(const Value &root, char separator):
    root_(root),
    separator_(separator) {
}

Configuration::Configuration(const Configuration &other):
    root_(other.root_),
    separator_(other.separator_) {
}

Configuration &Configuration::operator=(const Configuration &other)
{
    root_ = other.root_;
    separator_ = other.separator_;
    return *this;
}

Configuration::~Configuration() {
}


char Configuration::separator() const {
    return separator_;
}

eckit::Value Configuration::lookUp(const std::string &s, bool &found) const {

    eckit::Tokenizer parse(separator_);
    std::vector<std::string> path;
    parse(s, path);

    eckit::Value result = root_;

    // std::cout << "Configuration::lookUp root=" << result << std::endl;
    for (size_t i = 0; i < path.size(); i++) {
        const std::string &key = path[i];
        if (!result.contains(key)) {
            found = false;
            return result;
        }
        // std::cout << "Configuration::lookUp key=" << key  << std::endl;

        result = result[key];
        // std::cout << "Configuration::lookUp key=" << result  << std::endl;

    }

    found = true;
    return result;
}


eckit::Value Configuration::lookUp(const std::string &name) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    ASSERT(found);
    return v;
}


bool Configuration::has(const std::string &name) const {
    bool found = false;
    lookUp(name, found);
    return found;
}

bool Configuration::get(const std::string &name, std::string &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = std::string(v);
    }
    return found;
}

bool Configuration::get(const std::string &name, bool &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = v;
    }
    return found;
}

bool Configuration::get(const std::string &name, long &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = long(v);
    }
    return found;
}

bool Configuration::get(const std::string &name, double &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = v;
    }
    return found;
}

bool Configuration::get(const std::string &name, std::vector<long> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(v[i]);
            i++;
        }
    }
    return found;
}


bool Configuration::get(const std::string &name, std::vector<LocalConfiguration> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(LocalConfiguration(v[i], separator_));
            i++;
        }
    }
    return found;
}

bool Configuration::get(const std::string &name, std::vector<double> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(v[i]);
            i++;
        }
    }
    return found;
}

bool Configuration::get(const std::string &name, size_t &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = long(v);
    }
    return found;
}

}  // namespace eckit


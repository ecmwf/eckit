/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Jul 2015

#include <fstream>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"

namespace eckit {

static Value root(std::istream& in) {
    ASSERT(in);
    eckit::YAMLParser parser(in);
    Value root = parser.parse();
    return root;
}


static Value root(const std::string& path) {
    LOG_DEBUG_LIB(LibEcKit) << "Reading YAMLConfiguration from file " << path << std::endl;
    std::ifstream in(path.c_str());
    if (!in)
        throw eckit::CantOpenFile(path);
    return root(in);
}

static Value root(Stream& in) {
    std::string val;
    in.next(val);
    std::istringstream iss(val);
    return root(iss);
}

static Value root_from_string(const std::string& str) {
    LOG_DEBUG_LIB(LibEcKit) << "Reading YAMLConfiguration from string:" << std::endl;
    LOG_DEBUG_LIB(LibEcKit) << str << std::endl;
    std::istringstream in(str);
    return root(in);
}

YAMLConfiguration::YAMLConfiguration(const PathName& path, char separator) :
    Configuration(root(path), separator), path_(path) {}

YAMLConfiguration::YAMLConfiguration(std::istream& in, char separator) :
    Configuration(root(in), separator), path_("<istream>") {}

YAMLConfiguration::YAMLConfiguration(Stream& in, char separator) :
    Configuration(root(in), separator), path_("<Stream>") {}

YAMLConfiguration::YAMLConfiguration(const std::string& str, char separator) :
    Configuration(root_from_string(str), separator), path_("<string>") {}

YAMLConfiguration::YAMLConfiguration(const SharedBuffer& buffer, char separator) :
    Configuration(root_from_string(buffer.str()), separator), path_("<Buffer>") {}

YAMLConfiguration::~YAMLConfiguration() {}

void YAMLConfiguration::print(std::ostream& out) const {
    out << "YAMLConfiguration[path=" << path_ << ", root=" << *root_ << "]";
}

}  // namespace eckit

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/SemanticVersion.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

SemanticVersion::SemanticVersion() :
    major_(0), minor_(0), patch_(0) {}

SemanticVersion::SemanticVersion(unsigned long major, unsigned long minor, unsigned long patch) :
    major_(major), minor_(minor), patch_(patch) {}

static unsigned long s2int(const std::string& s) {
    if (s.size() == std::count_if(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); })) {
        unsigned long r = ::strtoul(s.data(), NULL, 0);
        if (r != ULONG_MAX) {
            return r;
        }
        std::ostringstream msg;
        msg << "Value out of range in version string '" << s << "'";
        throw eckit::OutOfRange(msg.str(), Here());
    }
    std::ostringstream msg;
    msg << "Bad stem in version string '" << s << "' - must contain only digits";
    throw eckit::BadValue(msg.str(), Here());
}

SemanticVersion::SemanticVersion(const std::string& v) {

    std::vector<std::string> tokens;
    eckit::Tokenizer('.', true)(v, tokens);

    if (tokens.size() != 3) {
        std::ostringstream msg;
        msg << "Bad version string '" << v << "' not in format x.y.z";
        throw BadValue(msg.str(), Here());
    }

    major_ = s2int(tokens[0]);
    minor_ = s2int(tokens[1]);
    patch_ = s2int(tokens[2]);
}

SemanticVersion::operator std::string() const {
    std::ostringstream msg;
    msg << major_ << "." << minor_ << "." << patch_;
    return msg.str();
}

bool SemanticVersion::operator<(const SemanticVersion& o) const {
    if (major_ == o.major_) {
        if (minor_ == o.minor_) {
            return (patch_ < o.patch_);
        }
        else {
            return (minor_ < o.minor_);
        }
    }
    return (major_ < o.major_);
}

bool SemanticVersion::operator==(const SemanticVersion& o) const {
    return (patch_ == o.patch_) && (minor_ == o.minor_) && (major_ == o.major_);
}

}  // namespace eckit

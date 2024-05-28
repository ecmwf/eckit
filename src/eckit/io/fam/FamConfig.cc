/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamConfig.h"

#include <ostream>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

bool FamConfig::operator==(const FamConfig& other) const {
    return (endpoint == other.endpoint && sessionName == other.sessionName);
}

std::ostream& operator<<(std::ostream& out, const FamConfig& config) {
    out << "endpoint=" << config.endpoint << ", sessionName=" << config.sessionName;
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

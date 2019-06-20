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
/// @author Tiago Quintino
/// @date   Dec 2018

#include "eckit/config/Parametrisation.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

Parametrisation::~Parametrisation() {}

bool Parametrisation::get(const std::string& name, long long& value) const {
    NOTIMP;
}

bool Parametrisation::get(const std::string& name, std::vector<long long>& value) const {
    NOTIMP;
}


}  // namespace eckit

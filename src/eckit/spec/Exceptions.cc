// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/spec/Exceptions.h"


namespace eckit::spec::exception {


SpecError::SpecError(const std::string& what, const CodeLocation& location) :
    Exception("SpecError: [" + what + "]", location) {}


}  // namespace eckit::spec::exception

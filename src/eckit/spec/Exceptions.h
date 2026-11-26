// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/exception/Exceptions.h"


namespace eckit::spec::exception {


class SpecError : public Exception {
public:

    explicit SpecError(const std::string&, const CodeLocation&);
};


}  // namespace eckit::spec::exception

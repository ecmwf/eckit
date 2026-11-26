// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2018

#ifndef eckit_serialisation_BadTag_h
#define eckit_serialisation_BadTag_h

#include "eckit/exception/Exceptions.h"

namespace eckit {

class BadTag : public eckit::Exception {
public:

    BadTag(const std::string& what);
};

}  // namespace eckit

#endif

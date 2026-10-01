// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date JUl 2015


#ifndef eckit_JSONConfiguration_H
#define eckit_JSONConfiguration_H

#warning eckit::JSONConfiguration is deprecated in favour of eckit::YAMLConfiguration found in header "eckit/config/YAMLConfiguration.h", as a drop-in replacement.

#include "eckit/config/YAMLConfiguration.h"

namespace eckit {

using JSONConfiguration = YAMLConfiguration;

}  // namespace eckit
#endif

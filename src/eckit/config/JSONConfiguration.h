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
/// @date JUl 2015


#ifndef eckit_JSONConfiguration_H
#define eckit_JSONConfiguration_H

#warning eckit::JSONConfiguration is deprecated in favour of eckit::YAMLConfiguration found in header "eckit/config/YAMLConfiguration.h", as a drop-in replacement.

#include "eckit/config/YAMLConfiguration.h"

namespace eckit {

typedef YAMLConfiguration JSONConfiguration;

}  // namespace eckit
#endif

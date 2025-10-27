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

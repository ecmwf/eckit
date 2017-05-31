/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Jun 2012

#ifndef eckit_YAMLParser_h
#define eckit_YAMLParser_h

#include "eckit/parser/ObjectParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class YAMLParser : public ObjectParser {

public: // methods

    YAMLParser(std::istream& in);

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

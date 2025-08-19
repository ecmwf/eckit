/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file IndentTarget.h
/// @author Baudouin Raoult

#ifndef eckit_log_IndentTarget_h
#define eckit_log_IndentTarget_h

#include <iosfwd>
#include <utility>

#include "eckit/log/PrefixTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class IndentTarget : public PrefixTarget {
public:

    IndentTarget(const std::string& prefix, LogTarget* target, const char* space = " ");

protected:

    void print(std::ostream& s) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

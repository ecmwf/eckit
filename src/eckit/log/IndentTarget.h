// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

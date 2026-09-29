// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file PrefixTarget.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_log_PrefixTarget_h
#define eckit_log_PrefixTarget_h

#include <iosfwd>
#include <string>
#include <utility>

#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PrefixTarget : public WrapperTarget {
public:

    PrefixTarget(const std::string& prefix, LogTarget* target = nullptr, const char* space = " ");

private:

    virtual void writePrefix();
    virtual void writeSuffix();

protected:

    void print(std::ostream& s) const;

protected:

    std::string prefix_;
    const char* space_;
    size_t prefixLength_;
    size_t spaceLength_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

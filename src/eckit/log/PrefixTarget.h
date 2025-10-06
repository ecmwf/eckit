/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file PrefixTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_PrefixTarget_h
#define eckit_log_PrefixTarget_h

#include <utility>

#include "eckit/log/WrapperTarget.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PrefixTarget : public WrapperTarget {
public:

    PrefixTarget(const std::string& prefix, LogTarget* target);

private:

    virtual void writePrefix();
    virtual void writeSuffix();

private:
    std::string prefix_;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file TimeStampTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_TimeStampTarget_h
#define eckit_log_TimeStampTarget_h

#include <utility>

#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TimeStampTarget : public WrapperTarget {
public:

    TimeStampTarget(const char* tag = "", LogTarget* target = 0);

    ~TimeStampTarget() override;

protected:

    void print(std::ostream& s) const override;

private:

    void writePrefix() override;
    void writeSuffix() override;

private:

    const char* tag_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

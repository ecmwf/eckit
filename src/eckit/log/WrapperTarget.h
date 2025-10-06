/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file WrapperTarget.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_log_WrapperTarget_h
#define eckit_log_WrapperTarget_h

#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class WrapperTarget : public LogTarget {

protected:  // methods

    WrapperTarget(LogTarget* target = 0);

    ~WrapperTarget() override;

protected:  // members

    LogTarget* target_;

private:

    void write(const char* start, const char* end) override;
    void flush() override;

    virtual void writePrefix() = 0;
    virtual void writeSuffix() = 0;

protected:

    bool prefix_;

    friend class ChannelBuffer;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file WrapperTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_WrapperTarget_h
#define eckit_log_WrapperTarget_h


#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class WrapperTarget : public LogTarget {
protected: // methods

    WrapperTarget(LogTarget* target = 0);
    ~WrapperTarget();

protected:
    LogTarget* target_;

private:

    virtual void write(const char* start, const char* end);
    virtual void flush();

    virtual void writePrefix() = 0;
    virtual void writeSuffix() = 0;

    bool prefix_;

    friend class ChannelBuffer;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

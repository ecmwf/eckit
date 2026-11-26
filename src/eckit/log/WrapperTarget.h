// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    WrapperTarget(LogTarget* target = nullptr);

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

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file LineBasedTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_LineBasedTarget_h
#define eckit_log_LineBasedTarget_h


#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LineBasedTarget : public LogTarget {

protected:  // methods

    LineBasedTarget();
    ~LineBasedTarget();

private:

    void write(const char* start, const char* end) override;
    void flush() override;
    virtual void line(const char* line) = 0;

    void reserve(size_t size);


    size_t size_;
    char* buffer_;
    size_t position_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

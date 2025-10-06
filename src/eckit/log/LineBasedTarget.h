/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

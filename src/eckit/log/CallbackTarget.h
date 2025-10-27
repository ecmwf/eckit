/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file CallbackTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_CallbackTarget_h
#define eckit_log_CallbackTarget_h

#include <iosfwd>
#include <utility>

#include "eckit/log/LineBasedTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class CallbackTarget : public LineBasedTarget {
public:

    typedef void (*callback_t)(void* ctxt, const char* msg);

    CallbackTarget(callback_t callback, void* context = 0);

    ~CallbackTarget() override;

private:

    void line(const char* line) override;

    void print(std::ostream& s) const override;

private:

    callback_t callback_;
    void* context_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

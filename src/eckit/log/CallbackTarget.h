// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    using callback_t = void (*)(void* ctxt, const char* msg);

    CallbackTarget(callback_t callback, void* context = nullptr);

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

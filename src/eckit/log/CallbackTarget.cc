// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/log/CallbackTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


CallbackTarget::CallbackTarget(callback_t callback, void* context) : callback_(callback), context_(context) {}

CallbackTarget::~CallbackTarget() {
    //    std::cerr << "CallbackTarget::~CallbackTarget()" << std::endl;
}

void CallbackTarget::line(const char* line) {
    callback_(context_, line);
}

void CallbackTarget::print(std::ostream& s) const {
    s << "CallbackTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

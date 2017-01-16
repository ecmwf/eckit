/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/CallbackTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


CallbackTarget::CallbackTarget(callback_t callback, void *context):
    callback_(callback),
    context_(context)
{
}

void CallbackTarget::line(const char* line) {
    callback_(context_, line);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

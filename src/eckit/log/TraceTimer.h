/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Timer.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_TraceTimer_h
#define eckit_TraceTimer_h

#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <class T>
class TraceTimer : public Timer {
public:

    explicit TraceTimer(const char* name) : Timer(name, eckit::Log::debug<T>()) {}

    explicit TraceTimer(const std::string& name) : Timer(name, eckit::Log::debug<T>()) {}
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

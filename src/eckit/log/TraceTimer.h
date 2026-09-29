// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

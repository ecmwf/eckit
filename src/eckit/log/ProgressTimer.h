// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File ProgressTimer.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_ProgressTimer_h
#define eckit_ProgressTimer_h

#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ProgressTimer : public Timer {
public:

    /// @param name of the timer, used for output
    /// @param limit counter maximum value
    /// @param unit counter unit (singular)
    /// @param countedProgress how often to output progress, based on count
    /// @param o output stream
    ProgressTimer(const std::string& name, size_t limit, const std::string& unit, size_t progressCounted = 10000,
                  std::ostream& o = Log::info());

    /// @param name of the timer, used for output
    /// @param limit counter maximum value
    /// @param unit counter unit (singular)
    /// @param timedProgress how often to output progress, based on elapsed time
    /// @param o output stream
    ProgressTimer(const std::string& name, size_t limit, const std::string& unit, double progressTimed = 10.,
                  std::ostream& o = Log::info());

    ProgressTimer& operator++();

    operator bool() const;

private:  // members

    const size_t limit_;
    const std::string unit_;

    const size_t progressCounted_;
    const double progressTimed_;

    size_t counter_;
    double lastTime_;
    bool hasOutput_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

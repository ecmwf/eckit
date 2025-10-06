/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

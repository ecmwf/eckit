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

#ifndef eckit_Timer_h
#define eckit_Timer_h

#include <sys/time.h>
#include <ctime>

#include "eckit/log/Log.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//-----------------------------------------------------------------------------

class Timer : private NonCopyable {
public:

    explicit Timer();

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit Timer(const std::string& name, std::ostream& o = Log::info());

    /// @param name of the timer, used for output
    /// @param o output stream to use  for output
    explicit Timer(const char* name, std::ostream& o = Log::info());

    ~Timer();

    void start();
    void stop();

    double elapsed();
    double elapsed_cpu();

    const std::string& name() const { return name_; }

    bool running() const { return !stopped_; }

    void report(const std::string& message = "");
    void reset(const std::string& message = "");

protected:  // methods

    void takeTime();

    std::ostream& output() { return out_ << name_ << ": "; }

private:  // members

    std::string name_;

    bool stopped_;
    bool outputAtExit_;

    struct ::timeval timeStart_;
    struct ::timeval timeStop_;

    clock_t cpuStart_;
    clock_t cpuStop_;

    std::ostream& out_;
};

//-----------------------------------------------------------------------------

::timeval operator-(const ::timeval&, const ::timeval&);

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

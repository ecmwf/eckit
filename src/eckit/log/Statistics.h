/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Statistics.h
/// @author Baudouin Raoult
/// @date   April 2016

#ifndef eckit_Statistics_H
#define eckit_Statistics_H

#include <iosfwd>

#include "eckit/log/Timer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;

struct Timing {
    double elapsed_;
    double cpu_;
    size_t updates_;

    Timing(): elapsed_(0), cpu_(0), updates_(0) {}
    Timing(double elapsed, double cpu, size_t updates): elapsed_(elapsed), cpu_(cpu), updates_(updates) {}
    Timing(Timer& timer): elapsed_(timer.elapsed()), cpu_(timer.elapsed_cpu()), updates_(1) {}
    Timing& operator+=(const Timing&);
    Timing operator-(const Timing&) const;
    Timing& operator/=(size_t);
};

Stream& operator>>(Stream&, Timing&);
Stream& operator<<(Stream&, const Timing&);
std::ostream& operator<<(std::ostream&, const Timing&);

class AutoTiming {
    Timer& timer_;
    Timing start_;
    Timing& timing_;
  public:
    AutoTiming(Timer& timer, Timing& timing):
        timer_(timer), start_(timer), timing_(timing) {}

    ~AutoTiming() {
        timing_ += Timing(timer_) - start_;
    }
};

//----------------------------------------------------------------------------------------------------------------------

class Statistics {
  public:
    static void reportUnit(std::ostream& out, const char* title, const char* unit, double value, const char* indent = "", bool always=false);
    static void reportRate(std::ostream& out, const char* title, unsigned long long value, const char* indent = "", bool always=false);
    static void reportCount(std::ostream& out, const char* title, size_t value, const char* indent = "", bool always=false);
    static void reportBytes(std::ostream& out, const char* title, unsigned long long value, const char* indent = "", bool always=false);
    static void reportTime(std::ostream& out, const char* title, const Timing& value, const char* indent = "", bool always=false);
    static void reportTime(std::ostream& out, const char* title, double value, const char* indent = "", bool always=false);
    static Timer timer_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

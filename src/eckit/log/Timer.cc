/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Timer::Timer() :
    name_("unnamed timer"), stopped_(true), outputAtExit_(false), cpuStart_(0), cpuStop_(0), out_(std::cout) {
    this->start();
}

Timer::Timer(const std::string& name, std::ostream& o) :
    name_(name), stopped_(true), outputAtExit_(true), cpuStart_(0), cpuStop_(0), out_(o) {
    this->start();
}

Timer::Timer(const char* name, std::ostream& o) :
    name_(name), stopped_(true), outputAtExit_(true), cpuStart_(0), cpuStop_(0), out_(o) {
    this->start();
}


Timer::~Timer() {
    stop();

    if (outputAtExit_)
        report();
}


void Timer::start() {
    if (!running()) {
        ::gettimeofday(&timeStart_, 0);
        timeStop_ = timeStart_;

        cpuStart_ = ::clock();
        cpuStop_  = cpuStart_;

        stopped_ = false;
    }
}


void Timer::stop() {
    if (!running())
        return;
    takeTime();
    stopped_ = true;
}


double Timer::elapsed() {
    if (running())
        takeTime();

    ::timeval diff = timeStop_ - timeStart_;
    return (double)diff.tv_sec + ((double)diff.tv_usec / 1000000.);
}

double Timer::elapsed_cpu() {
    if (running())
        takeTime();

    return double(cpuStop_ - cpuStart_) / CLOCKS_PER_SEC;
}

void Timer::report(const std::string& message) {
    const double s   = elapsed();
    const double cpu = elapsed_cpu();

    out_ << (message.size() ? message : name_) << ": " << Seconds(s) << " elapsed, " << Seconds(cpu) << " cpu"
         << std::endl;
}

void Timer::takeTime() {
    cpuStop_ = ::clock();
    ::gettimeofday(&timeStop_, 0);
}

//----------------------------------------------------------------------------------------------------------------------

::timeval operator-(const ::timeval& a, const ::timeval& b) {
    ::timeval diff;

    diff.tv_sec  = a.tv_sec - b.tv_sec;
    diff.tv_usec = a.tv_usec - b.tv_usec;

    if (diff.tv_usec < 0) {
        diff.tv_sec--;
        diff.tv_usec += 1000000;
    }

    return diff;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

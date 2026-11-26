// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    if (outputAtExit_) {
        report();
    }
}


void Timer::start() {
    if (!running()) {
        ::gettimeofday(&timeStart_, nullptr);
        timeStop_ = timeStart_;

        cpuStart_ = ::clock();
        cpuStop_  = cpuStart_;

        stopped_ = false;
    }
}


void Timer::stop() {
    if (!running()) {
        return;
    }
    takeTime();
    stopped_ = true;
}


double Timer::elapsed() {
    if (running()) {
        takeTime();
    }

    ::timeval diff = timeStop_ - timeStart_;
    return (double)diff.tv_sec + ((double)diff.tv_usec / 1000000.);
}

double Timer::elapsed_cpu() {
    if (running()) {
        takeTime();
    }

    return double(cpuStop_ - cpuStart_) / CLOCKS_PER_SEC;
}

void Timer::report(const std::string& message) {
    const double s   = elapsed();
    const double cpu = elapsed_cpu();

    out_ << (message.size() ? message : name_) << ": " << Seconds(s) << " elapsed, " << Seconds(cpu) << " cpu"
         << std::endl;
}

void Timer::reset(const std::string& message) {
    stop();
    report(message);
    start();
}

void Timer::takeTime() {
    cpuStop_ = ::clock();
    ::gettimeofday(&timeStop_, nullptr);
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

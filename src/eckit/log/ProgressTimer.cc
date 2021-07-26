/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/ProgressTimer.h"
#include "eckit/log/ETA.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Seconds.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ProgressTimer::ProgressTimer(const std::string& name, size_t limit, const std::string& unit, size_t progressCounted,
                             std::ostream& o) :
    Timer(name, o),
    limit_(limit),
    unit_(unit),
    progressCounted_(progressCounted),
    progressTimed_(-1),
    counter_(0),
    lastTime_(0.) {}

ProgressTimer::ProgressTimer(const std::string& name, size_t limit, const std::string& unit, double progressTimed,
                             std::ostream& o) :
    Timer(name, o),
    limit_(limit),
    unit_(unit),
    progressCounted_(0),
    progressTimed_(progressTimed),
    counter_(0),
    lastTime_(0.) {}

ProgressTimer& ProgressTimer::operator++() {
    hasOutput_ = counter_ && ((progressCounted_ > 0 && counter_ % progressCounted_ == 0) || (progressTimed_ > 0. && lastTime_ + progressTimed_ < elapsed()));

    if (hasOutput_) {
        lastTime_   = elapsed();
        double rate = counter_ / lastTime_;
        output() << eckit::Plural(counter_, unit_) << " in " << eckit::Seconds(lastTime_) << ", rate: " << rate << " "
                 << unit_ << "s/s"
                 << ", ETA: " << eckit::ETA((limit_ - counter_) / rate) << std::endl;
    }

    if (counter_ < limit_) {
        ++counter_;
    }

    return *this;
}

eckit::ProgressTimer::operator bool() const {
    return hasOutput_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

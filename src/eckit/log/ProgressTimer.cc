// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
    hasOutput_ = counter_ && ((progressCounted_ > 0 && counter_ % progressCounted_ == 0) ||
                              (progressTimed_ > 0. && lastTime_ + progressTimed_ < elapsed()));

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

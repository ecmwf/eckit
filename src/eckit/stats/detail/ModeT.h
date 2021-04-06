/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_ModeT_h
#define mir_stats_detail_ModeT_h

#include <cmath>
#include <limits>
#include <map>
#include <vector>

#include "mir/stats/ValueStatistics.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
}  // namespace mir


namespace mir {
namespace stats {
namespace detail {


/**
 * Mode statistics
 * @note: Multi-modal results are resolved with either maximum/minimum bin
 * @note: Boolean specialisation (eg. masks) is implemented as a Boyer-moore majority vote algorithm (2-bin)
 */
template <typename T>
struct Mode : ValueStatistics {
    Mode(bool disambiguateMax) : disambiguateMax_(disambiguateMax) {}

    virtual void operator()(const double&) = 0;
    virtual void operator()(const float&)  = 0;
    virtual void operator()(const int&)    = 0;
    virtual double mode() const            = 0;

    void reset() { binCount_.clear(); }

protected:
    T bin() const {
        ASSERT(!binCount_.empty());
        auto i = binCount_.begin();
        auto j = i++;
        for (; i != binCount_.end(); ++i) {
            if ((j->second < i->second) || (j->second == i->second && disambiguateMax_)) {
                j = i;
            }
        }
        return j->first;
    }

    std::map<T, size_t> binCount_;
    bool disambiguateMax_;
};


struct ModeIntegral : Mode<int> {
    ModeIntegral(bool disambiguateMax);
    ModeIntegral(const param::MIRParametrisation&);

    void operator()(const double& value) override { binCount_[static_cast<int>(std::lround(value))]++; }
    void operator()(const float& value) override { binCount_[static_cast<int>(std::lround(value))]++; }
    void operator()(const int& value) override { binCount_[value]++; }

    void operator+=(const ModeIntegral& other) {
        for (auto& bc : other.binCount_) {
            binCount_[bc.first] += bc.second;
        }
    }

    double mode() const override {
        return binCount_.empty() ? std::numeric_limits<double>::quiet_NaN() : static_cast<double>(bin());
    }

    void print(std::ostream&) const override;
};


struct ModeReal : Mode<size_t> {
    ModeReal(bool disambiguateMax, const std::vector<double>& values = {0, 1}, const std::vector<double>& mins = {0.5});
    ModeReal(const param::MIRParametrisation&);

    void operator()(const double& value) override {
        size_t bin = 0;
        while (bin < mins_.size() && mins_[bin] < value) {
            ++bin;
        }
        binCount_[bin]++;
    }

    void operator()(const float& value) override { operator()(static_cast<double>(value)); }

    void operator()(const int& value) override { operator()(static_cast<double>(value)); }

    void operator+=(const ModeReal& other) {
        for (auto& bc : other.binCount_) {
            binCount_[bc.first] += bc.second;
        }
    }

    double mode() const override {
        if (binCount_.empty()) {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return values_.at(bin());
    }

    void print(std::ostream&) const override;

    std::vector<double> values_;
    std::vector<double> mins_;
};


struct ModeBoolean : ValueStatistics {
    ModeBoolean(bool disambiguateMax, double min = 0.5);
    ModeBoolean(const param::MIRParametrisation&);

    void operator()(const double& value) override {
        set_ = true;
        if (min_ <= value) {
            majority_++;
        }
        else {
            majority_--;
        }
    }

    void operator+=(const ModeBoolean& other) { majority_ += other.majority_; }

    double mode() const {
        if (!set_) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return majority_ > 0 || (majority_ == 0 && disambiguateMax_) ? 1 : 0;
    }

    void reset() {
        set_      = false;
        majority_ = 0;
    }

    void print(std::ostream&) const override;

    long long majority_;
    double min_;
    bool disambiguateMax_;
    bool set_;
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif

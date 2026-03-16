/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/log/Bytes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct Candidate {

    const PathName* path_;

    FileSystemSize size_;

    double probability_;

    Candidate(const PathName* path) : path_(path) {}

    void print(std::ostream& s) const {
        s << "Candidate(path=" << path_->asString() << ",total=" << total() << ",available=" << available()
          << ",availablePercent=" << availablePercent() << ",probability=" << probability_ << ")";
    }

    friend std::ostream& operator<<(std::ostream& s, const Candidate& v) {
        v.print(s);
        return s;
    }

    const PathName& path() const { return *path_; }

    double probability() const { return probability_; }
    void probability(double p) { probability_ = p; }

    long usedPercent() const {
        ASSERT(size_.total > 0);
        return long(100. * (double(size_.total - size_.available) / size_.total));
    }
    long availablePercent() const {
        ASSERT(size_.total > 0);
        return long(100. * (double(size_.available) / size_.total));
    }

    unsigned long long total() const { return size_.total; }

    unsigned long long available() const { return size_.available; }
};

//----------------------------------------------------------------------------------------------------------------------

const PathName& FileSpaceStrategies::selectFileSystem(const std::vector<PathName>& fileSystems,
                                                      const std::string& strategy) {
    Log::info() << "FileSpace::selectFileSystem is " << strategy << std::endl;

    if (strategy == "roundRobin") {
        return FileSpaceStrategies::roundRobin(fileSystems);
    }

    if (strategy == "weightedRandom") {
        return FileSpaceStrategies::weightedRandom(fileSystems);
    }

    if (strategy == "pureRandom") {
        return FileSpaceStrategies::pureRandom(fileSystems);
    }

    if (strategy == "weightedRandomPercent") {
        return FileSpaceStrategies::weightedRandomPercent(fileSystems);
    }

    if (strategy == "leastUsedPercent") {
        return FileSpaceStrategies::leastUsedPercent(fileSystems);
    }

    if (strategy == "binnedLeastUsed") {
        return FileSpaceStrategies::binnedLeastUsed(fileSystems);
    }

    return FileSpaceStrategies::leastUsed(fileSystems);
}

const PathName& FileSpaceStrategies::leastUsed(const std::vector<PathName>& fileSystems) {
    unsigned long long free = 0;
    Ordinal best            = 0;
    Ordinal checked         = 0;

    ASSERT(fileSystems.size() != 0);

    for (Ordinal i = 0; i < fileSystems.size(); i++) {
        // Log::info() << "leastUsed: " << fileSystems[i] << " " << fileSystems[i].available() << std::endl;
        if (fileSystems[i].available()) {
            FileSystemSize fs;

            try {
                fileSystems[i].fileSystemSize(fs);
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if (fs.available >= free || checked == 0) {
                free = fs.available;
                best = i;
                checked++;
            }
        }
    }

    if (!checked) {
        throw Retry(std::string("No available filesystem (") + fileSystems[0] + ")");
    }

    Log::info() << "Filespace strategy leastUsed selected " << fileSystems[best] << " " << Bytes(free) << " available"
                << std::endl;

    return fileSystems[best];
}

const PathName& FileSpaceStrategies::leastUsedPercent(const std::vector<PathName>& fileSystems) {
    long bestPercent = 0;
    size_t best      = 0;
    size_t checked   = 0;

    ASSERT(fileSystems.size() != 0);

    for (size_t i = 0; i < fileSystems.size(); ++i) {
        Candidate candidate(&fileSystems[i]);

        Log::info() << "leastUsedPercent: " << fileSystems[i] << " " << fileSystems[i].available() << std::endl;
        if (fileSystems[i].available()) {

            try {
                fileSystems[i].fileSystemSize(candidate.size_);
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if (candidate.total() == 0) {
                Log::error() << "Cannot get total size of " << fileSystems[i] << std::endl;
                continue;
            }

            if (candidate.availablePercent() >= bestPercent || checked == 0) {
                bestPercent = candidate.availablePercent();
                best        = i;
                checked++;
            }
        }
    }

    if (!checked) {
        throw Retry(std::string("No available filesystem (") + fileSystems[0] + ")");
    }

    Log::info() << "Filespace strategy leastUsedPercent selected " << fileSystems[best] << " " << bestPercent
                << "% available" << std::endl;

    return fileSystems[best];
}

using compute_probability_t = void (*)(Candidate&);

static void computePercent(Candidate& c) {
    c.probability_ = double(c.availablePercent());
}

static void computeAvailable(Candidate& c) {
    c.probability_ = double(c.available());
}

static void computeIdentity(Candidate& c) {
    c.probability_ = 1;
}

static void computeNull(Candidate& c) {
    c.probability_ = 0;
}

static std::vector<Candidate> findCandidates(const std::vector<PathName>& fileSystems,
                                             compute_probability_t probability) {

    ASSERT(fileSystems.size() != 0);

    static Resource<long> candidateFileSystemMaxUsedPercent("candidateFileSystem", 99);

    std::vector<Candidate> result;

    for (size_t i = 0; i < fileSystems.size(); ++i) {

        Candidate candidate(&fileSystems[i]);

        if (fileSystems[i].available()) {

            try {
                fileSystems[i].fileSystemSize(candidate.size_);
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if (candidate.total() == 0) {
                Log::warning() << "Cannot get total size of " << fileSystems[i] << std::endl;
                continue;
            }

            if (candidate.usedPercent() <= candidateFileSystemMaxUsedPercent) {

                probability(candidate);

                //                Log::info() << candidate << std::endl;

                result.push_back(candidate);
            }
        }
    }

    return result;
}

const PathName& FileSpaceStrategies::roundRobin(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computeNull);

    if (candidates.empty()) {
        return leastUsed(fileSystems);
    }

    static long value = -1;

    if (value < 0) {
        value = ::getpid();
    }

    value++;
    value %= candidates.size();

    Log::info() << "Filespace strategy roundRobin selected " << candidates[value].path() << " " << value << " out of "
                << candidates.size() << std::endl;

    return candidates[value].path();
}

static void attenuateProbabilities(std::vector<Candidate>& candidates) {

    ASSERT(!candidates.empty());

    static double attenuation = Resource<double>("attenuateFileSpacePeakProbability", 0.);

    ASSERT(attenuation >= 0.);
    ASSERT(attenuation <= 1.);

    if (attenuation == 0.) {
        return;
    }

    // compute mean

    double mean = 0.;
    for (const auto& c : candidates) {
        mean += c.probability();
    }

    mean /= candidates.size();

    //    // compute variance

    //    double variance = 0.;
    //    for(std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
    //        double diff = (i->probability() - mean);
    //        variance += diff*diff;
    //    }

    //    variance /= candidates.size();

    //    // compute stddev

    //    double stddev = std::sqrt(variance);

    //    // attenuate the peaks that exceed the stddev to the stddev value
    //    double max = mean + attenuation * stddev;
    //    for(std::vector<Candidate>::iterator i = candidates.begin(); i != candidates.end(); ++i) {
    //        if(i->probability() > max) {
    //            i->probability(max);
    //        }
    //    }


    for (auto& c : candidates) {
        double p    = c.probability();
        double newp = attenuation * mean + (1. - attenuation) * p;
        c.probability(newp);
    }
}


static const PathName& chooseByProbability(const char* strategy, const std::vector<Candidate>& candidates) {

    double total = 0;
    for (const auto& c : candidates) {
        total += c.probability();
    }

    double choice = (double(random()) / double(RAND_MAX));

    choice *= total;

    std::vector<Candidate>::const_iterator select = candidates.begin();

    double lower = 0;
    double upper = 0;
    for (auto i = candidates.begin(); i != candidates.end(); ++i) {

        upper += i->probability();

        if (choice >= lower && choice < upper) {
            select = i;
            break;
        }

        lower = upper;
    }

    Log::info() << "Filespace strategy " << strategy << " selected " << select->path() << " "
                << Bytes(select->available()) << " available" << std::endl;

    return select->path();
}

const PathName& FileSpaceStrategies::pureRandom(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computeIdentity);

    if (candidates.empty()) {
        return leastUsed(fileSystems);
    }

    attenuateProbabilities(candidates); /* has no effect */

    return chooseByProbability("pureRandom", candidates);
}

const PathName& FileSpaceStrategies::weightedRandom(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computeAvailable);

    if (candidates.empty()) {
        return leastUsed(fileSystems);
    }

    attenuateProbabilities(candidates);

    return chooseByProbability("weightedRandom", candidates);
}

const PathName& FileSpaceStrategies::weightedRandomPercent(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computePercent);

    if (candidates.empty()) {
        return leastUsed(fileSystems);
    }

    attenuateProbabilities(candidates);

    return chooseByProbability("weightedRandomPercent", candidates);
}

const PathName& FileSpaceStrategies::binnedLeastUsed(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computeNull);

    if (candidates.empty()) {
        return leastUsed(fileSystems);
    }

    static Resource<long> numBins("binnedLeastUsedNumBins", 20);
    ASSERT(numBins > 0);

    auto [minIt, maxIt] =
        std::minmax_element(candidates.begin(), candidates.end(),
                            [](const Candidate& a, const Candidate& b) { return a.available() < b.available(); });

    size_t maxAvail  = maxIt->available();
    size_t minAvail  = minIt->available();
    size_t threshold = maxAvail - ((maxAvail - minAvail) / static_cast<size_t>(numBins));

    for (auto& c : candidates) {
        c.probability_ = (c.available() >= threshold) ? 1.0 : 0.0;
    }

    return chooseByProbability("binnedLeastUsed", candidates);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

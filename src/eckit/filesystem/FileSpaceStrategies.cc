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

    Candidate(const PathName* path) :
        path_(path) {}

    void print(std::ostream& s) const {
        s << "Candidate(path=" << path_->asString() << ",total=" << total() << ",available=" << available()
          << ",percent=" << percent() << ",probability=" << probability_ << ")";
    }

    friend std::ostream& operator<<(std::ostream& s, const Candidate& v) {
        v.print(s);
        return s;
    }

    const PathName& path() const { return *path_; }

    double probability() const { return probability_; }
    void probability(double p) { probability_ = p; }

    long percent() const { return long(100. * (double(size_.available) / size_.total)); }

    unsigned long long total() const { return size_.total; }

    unsigned long long available() const { return size_.available; }
};

//----------------------------------------------------------------------------------------------------------------------

const PathName& FileSpaceStrategies::selectFileSystem(const std::vector<PathName>& fileSystems, const std::string& s) {
    Log::info() << "FileSpace::selectFileSystem is " << s << std::endl;

    if (s == "roundRobin")
        return FileSpaceStrategies::roundRobin(fileSystems);

    if (s == "weightedRandom")
        return FileSpaceStrategies::weightedRandom(fileSystems);

    if (s == "pureRandom")
        return FileSpaceStrategies::pureRandom(fileSystems);

    if (s == "weightedRandomPercent")
        return FileSpaceStrategies::weightedRandomPercent(fileSystems);

    if (s == "leastUsedPercent")
        return FileSpaceStrategies::leastUsedPercent(fileSystems);

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

    if (!checked)
        throw Retry(std::string("No available filesystem (") + fileSystems[0] + ")");

    Log::info() << "Filespace strategy leastUsed selected " << fileSystems[best] << " " << Bytes(free) << " available"
                << std::endl;

    return fileSystems[best];
}

const PathName& FileSpaceStrategies::leastUsedPercent(const std::vector<PathName>& fileSystems) {
    long percent = 0;
    size_t best  = 0;

    ASSERT(fileSystems.size() != 0);

    for (size_t i = 0; i < fileSystems.size(); ++i) {
        Candidate candidate(&fileSystems[i]);

        Log::info() << "leastUsedPercent: " << fileSystems[i] << " " << fileSystems[i].available() << std::endl;
        if (fileSystems[i].available()) {
            FileSystemSize fs;

            try {
                fileSystems[i].fileSystemSize(candidate.size_);
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if (candidate.percent() >= percent) {
                percent = candidate.percent();
                best    = i;
            }
        }
    }

    Log::info() << "Filespace strategy leastUsedPercent selected " << fileSystems[best] << " " << percent
                << "% available" << std::endl;

    return fileSystems[best];
}

typedef void (*compute_probability_t)(Candidate&);

static void computePercent(Candidate& c) {
    c.probability_ = double(c.percent());
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

    static Resource<long> candidateFileSystemPercent("candidateFileSystem", 99);

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
                return std::vector<Candidate>();
            }

            if (candidate.percent() <= candidateFileSystemPercent) {

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

    if (candidates.empty())
        return leastUsed(fileSystems);

    static long value = -1;

    if (value < 0)
        value = ::getpid();

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

    if (attenuation == 0.)
        return;

    // compute mean

    double mean = 0.;
    for (std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        mean += i->probability();
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


    for (std::vector<Candidate>::iterator i = candidates.begin(); i != candidates.end(); ++i) {
        double p    = i->probability();
        double newp = attenuation * mean + (1. - attenuation) * p;
        i->probability(newp);
    }
}


static const PathName& chooseByProbabylity(const char* strategy, const std::vector<Candidate>& candidates) {

    double total = 0;
    for (std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        //        Log::info() << "probability " << i->probability() << std::endl;
        total += i->probability();
    }

    double choice = (double(random()) / double(RAND_MAX));

    //    Log::info() << "choice " << choice << std::endl;

    choice *= total;

    std::vector<Candidate>::const_iterator select = candidates.begin();

    double lower = 0;
    double upper = 0;
    for (std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {

        upper += i->probability();

        //        Log::info() << "Choice " << choice << " total = " << total << " lower = " << lower << " upper = " <<
        //        upper << std::endl;

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

    if (candidates.empty())
        return leastUsed(fileSystems);

    attenuateProbabilities(candidates); /* has no effect */

    return chooseByProbabylity("pureRandom", candidates);
}

const PathName& FileSpaceStrategies::weightedRandom(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computeAvailable);

    if (candidates.empty())
        return leastUsed(fileSystems);

    attenuateProbabilities(candidates);

    return chooseByProbabylity("weightedRandom", candidates);
}

const PathName& FileSpaceStrategies::weightedRandomPercent(const std::vector<PathName>& fileSystems) {
    std::vector<Candidate> candidates = findCandidates(fileSystems, &computePercent);

    if (candidates.empty())
        return leastUsed(fileSystems);

    attenuateProbabilities(candidates);

    return chooseByProbabylity("weightedRandomPercent", candidates);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

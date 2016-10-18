/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/log/Bytes.h"
#include "eckit/config/Resource.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct Candidate {

    const PathName* path_;
    FileSystemSize size_;

    Candidate(const PathName* path) : path_(path) {}
    Candidate(const Candidate& other) : path_(other.path_), size_(other.size_) {}

    Candidate& operator=(const Candidate& other) {
        path_ = other.path_;
        size_ = other.size_;
        return *this;
    }

    const PathName& path() const { return *path_; }
    long percent() const { return long( ( (double(size_.total) - double(size_.available)) / size_.total * 100) + 0.5); }
    unsigned long long total() const { return size_.total; }
    unsigned long long available() const { return size_.available; }
};

//----------------------------------------------------------------------------------------------------------------------

const PathName& FileSpaceStrategies::selectFileSystem(const std::vector<PathName>& fileSystems, const std::string& s)
{
    Log::info() << "FileSpace::selectFileSystem is " << s << std::endl;

    if(s == "roundRobin")
        return FileSpaceStrategies::roundRobin(fileSystems);

    if(s == "weightedRandom")
        return FileSpaceStrategies::weightedRandom(fileSystems);

    if(s == "pureRandom")
        return FileSpaceStrategies::pureRandom(fileSystems);

    if(s == "weightedRandomPercent")
        return FileSpaceStrategies::weightedRandomPercent(fileSystems);

    if(s == "leastUsedPercent")
        return FileSpaceStrategies::leastUsedPercent(fileSystems);

    return FileSpaceStrategies::leastUsed(fileSystems);
}

const PathName& FileSpaceStrategies::leastUsed(const std::vector<PathName>& fileSystems)
{
	unsigned long long free = 0;
	Ordinal best = 0;
	Ordinal checked = 0;

	ASSERT(fileSystems.size() != 0);

	for(Ordinal i = 0; i < fileSystems.size(); i++)
	{
		Log::info() << "leastUsed: " << fileSystems[i] << " " << fileSystems[i].available() << std::endl;
		if(fileSystems[i].available())
		{
			FileSystemSize fs;

			try
			{
				fileSystems[i].fileSystemSize(fs);
			}
			catch(std::exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
				Log::error() << "** Exception is ignored" << std::endl;
				Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
				continue;
			}

			if(fs.available >= free || checked == 0)
			{
				free = fs.available;
				best = i;
				checked++;
			}

		}
	}

	if(!checked)
        throw Retry(std::string("No available filesystem (") + fileSystems[0] + ")");

	Log::info() << "Least used file system: " << fileSystems[best] << " " << Bytes(free) << " available" << std::endl;

	return fileSystems[best];
}

const PathName& FileSpaceStrategies::leastUsedPercent(const std::vector<PathName>& fileSystems)
{
    long percent = 0;
    size_t best = 0;

    ASSERT(fileSystems.size() != 0);

    for(size_t i = 0; i < fileSystems.size(); ++i)
    {
        Candidate candidate(&fileSystems[i]);

        Log::info() << "leastUsedPercent: " << fileSystems[i] << " " << fileSystems[i].available() << std::endl;
        if(fileSystems[i].available())
        {
            FileSystemSize fs;

            try
            {
                fileSystems[i].fileSystemSize(candidate.size_);
            }
            catch(std::exception& e)
            {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if(candidate.percent() >= percent)
            {
                percent = candidate.percent();
                best = i;
            }
        }
    }

    Log::info() << "Least used (percent) file system: " << fileSystems[best] << " " << percent << "% available" << std::endl;

    return fileSystems[best];
}

static std::vector<Candidate> findCandidates(const std::vector<PathName>& fileSystems) {

    ASSERT(fileSystems.size() != 0);

    static Resource<long> candidateFileSystemPercent("candidateFileSystem", 99);

    std::vector<Candidate> result;

    for(size_t i = 0; i < fileSystems.size(); ++i) {

        Candidate candidate(&fileSystems[i]);

        if(fileSystems[i].available()) {

            try
            {
                fileSystems[i].fileSystemSize(candidate.size_);
            }
            catch(std::exception& e)
            {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
                Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << std::endl;
                continue;
            }

            if(candidate.total() == 0) {
                Log::warning() << "Cannot get total size of " << fileSystems[i] << std::endl;
                return std::vector<Candidate>();
            }

            if(candidate.percent() <= candidateFileSystemPercent) {
                Log::info() << "Candidate filesystem " << fileSystems[i] << " total = " << candidate.total() << " free = " << candidate.available() << std::endl;
                result.push_back(candidate);
            }
        }
    }

    return result;
}

const PathName& FileSpaceStrategies::roundRobin(const std::vector<PathName>& fileSystems)
{
    std::vector<Candidate> candidates = findCandidates(fileSystems);

    if(candidates.empty())
        return leastUsed(fileSystems);

    static long value = -1;

	if(value < 0)
		value = ::getpid();

	value++;
    value %= candidates.size();

    Log::info() << "roundRobin selection " << value << " out of " << candidates.size() << std::endl;

    return candidates[value].path();
}

const PathName& FileSpaceStrategies::pureRandom(const std::vector<PathName>& fileSystems)
{
    std::vector<Candidate> candidates = findCandidates(fileSystems);

    if(candidates.empty())
        return leastUsed(fileSystems);

    double choice = double(random()) / double(RAND_MAX);

    size_t select = size_t(choice * candidates.size()) % candidates.size();

    Log::info() << "choice = " << choice << " candidates.size() = " <<  candidates.size() << " select = " << select << std::endl;

    return candidates[select].path();
}

const PathName& FileSpaceStrategies::weightedRandom(const std::vector<PathName>& fileSystems)
{
    const std::vector<Candidate> candidates = findCandidates(fileSystems);

    if(candidates.empty())
        return leastUsed(fileSystems);

    size_t free_space = 0;
    for(std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        free_space += i->available();
    }

    double choice = double(random()) / double(RAND_MAX);

    choice *= free_space;

    std::vector<Candidate>::const_iterator select = candidates.begin();

    double lower = 0.;
    double upper = 0.;


    for(std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {

        upper += double(i->available());

//      Log::info() << "Choice " << choice << " free_space = " << free_space << " lower = " << lower << " upper = " << upper << std::endl;

        if(choice >= lower && choice < upper) {
            select = i;
        }

        lower = upper;
    }

    Log::info() << "Weighted random file system: " <<  select->path() << " " << Bytes(select->available()) << " available" << std::endl;

    return select->path();
}

const PathName& FileSpaceStrategies::weightedRandomPercent(const std::vector<PathName>& fileSystems)
{
    const std::vector<Candidate> candidates = findCandidates(fileSystems);

    if(candidates.empty())
        return leastUsed(fileSystems);

    long free_percent = 0;
    for(std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        free_percent += i->percent();
    }

    double choice = double(random()) / double(RAND_MAX);

    choice *= free_percent;

    std::vector<Candidate>::const_iterator select = candidates.begin();

    double lower = 0.;
    double upper = 0.;

    for(std::vector<Candidate>::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {

        upper += double(i->percent());

//      Log::info() << "Choice " << choice << " free_percent = " << free_percent << " lower = " << lower << " upper = " << upper << std::endl;

        if(choice >= lower && choice < upper) {
            select = i;
        }

        lower = upper;
    }

    Log::info() << "Weighted random (percent) file system: " <<  select->path() << " " << select->percent() << "% available" << std::endl;

    return select->path();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

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

const PathName&FileSpaceStrategies::selectFileSystem(const std::vector<PathName>& fileSystems, const std::string& s)
{
    Log::info() << "FileSpace::selectFileSystem is " << s << std::endl;

    if(s == "roundRobin")
        return FileSpaceStrategies::roundRobin(fileSystems);
    if(s == "weightedRandom")
        return FileSpaceStrategies::weightedRandom(fileSystems);
    if(s == "pureRandom")
        return FileSpaceStrategies::pureRandom(fileSystems);

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

const PathName& FileSpaceStrategies::roundRobin(const std::vector<PathName>& fileSystems)
{
	static long value = -1;
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

	if(value < 0)
		value = ::getpid();

	value++;
	value %= fileSystems.size();

	Log::info() << "roundRobin selection " << value << " out of " << fileSystems.size() << std::endl;

	for(Ordinal j = 0; j < fileSystems.size(); j++)
	{
		Ordinal i = (j + value) % fileSystems.size();
		if(fileSystems[j].available())
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

			if(fs.total == 0)
			{
				Log::info() << "roundRobin: cannot get total size of " << fileSystems[i] << std::endl;
				return leastUsed(fileSystems);
			}

			long percent = long(((double) (fs.total - fs.available) / fs.total * 100) + 0.5);

#if 0
			Log::info() << "roundRobin: percent=" << percent << " candidate=" << candidate <<
			" total="<< total << " " << Bytes(total) <<
			" size="<< size <<
			endl;
#endif

			if(percent <= candidate)
			{
				value = i;
				Log::info() << "Round robin file system: " << fileSystems[value] << " " << Bytes(fs.available) << " available" << std::endl;
				return fileSystems[value];

			}

		}
	}

	Log::info() << "roundRobin reverting to leastUsed" << std::endl;

	return leastUsed(fileSystems);

}

const PathName& FileSpaceStrategies::pureRandom(const std::vector<PathName>& fileSystems)
{
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

	long value = random() % fileSystems.size();

    for(Ordinal j = 0; j < fileSystems.size(); j++) {

        Ordinal i = (j + value) % fileSystems.size();

        if(fileSystems[i].available()) {

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

			if(fs.total == 0)
				return leastUsed(fileSystems);

			long percent = long(((double) (fs.total - fs.available) / fs.total * 100) + 0.5);

			if(percent <= candidate)
			{
				value = i;
				Log::info() << "Pure random file system: " << fileSystems[value] << " " << Bytes(fs.available) << " available" << std::endl;
				return fileSystems[value];
			}

		}
	}

	return leastUsed(fileSystems);

}

const PathName& FileSpaceStrategies::weightedRandom(const std::vector<PathName>& fileSystems)
{
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

    std::map<size_t, unsigned long long> scores;
    double free_space = 0;

    for(size_t i = 0; i < fileSystems.size(); ++i) {

        if(fileSystems[i].available()) {

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

			if(fs.total == 0)
				return leastUsed(fileSystems);

            long percent = long( ( (double(fs.total) - double(fs.available)) / fs.total * 100) + 0.5);

			if(percent <= candidate)
			{
				scores[i] = fs.available;
                free_space += double(fs.available);
			}

		}
    }

    if(scores.empty()) { return leastUsed(fileSystems); }

    double choice = double(random()) / double(RAND_MAX);

    choice *= free_space;

    size_t value = (*scores.begin()).first;

    double lower = 0.;
    double upper = 0.;

    for(std::map<size_t, unsigned long long>::iterator j = scores.begin(); j != scores.end(); ++j) {

        upper += double((*scores.begin()).second);

//      Log::info() << "Choice " << choice << " free_space = " << free_space << " lower = " << lower << " upper = " << upper << std::endl;

        if(choice >= lower && choice < upper) {
			value = (*j).first;
        }

        lower = upper;
	}

	Log::info() << "Weighted random file system: " << fileSystems[value] << " " << Bytes(scores[value]) << " available" << std::endl;

	return fileSystems[value];
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

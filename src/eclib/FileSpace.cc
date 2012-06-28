/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/AutoLock.h"
#include "eclib/Bytes.h"
#include "eclib/ClusterDisks.h"
#include "eclib/FileSpace.h"
#include "eclib/Mutex.h"
#include "eclib/Resource.h"
#include "eclib/Types.h"

typedef map<string, FileSpace*> Map;

CREATE_MUTEX();

static Map space;

FileSpace::FileSpace(const string& name) :
	name_(name), last_(0)
{
    INIT_MUTEX();    
	AutoLock<Mutex> lock(mutex);
	space[name] = this;
	load();
}

FileSpace::~FileSpace()
{
	AutoLock<Mutex> lock(mutex);
	space.erase(name_);
}

static const PathName& leastUsed(const vector<PathName>& fileSystems)
{
	static const char *here = __FUNCTION__;
	long long free = -1;
	Ordinal best = 0;
	Ordinal checked = 0;

	ASSERT(fileSystems.size() != 0);

	for(Ordinal i = 0; i < fileSystems.size(); i++)
	{
		Log::info() << "leastUsed: " << fileSystems[i] << " " << fileSystems[i].available() << endl;
		if(fileSystems[i].available())
		{
			FileSystemSize fs;

			try
			{
				fileSystems[i].fileSystemSize(fs);
			}
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " << here << endl;
				Log::error() << "** Exception is ignored" << endl;
				Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << endl;
				continue;
			}

			if(fs.available > free || checked == 0)
			{
				free = fs.available;
				best = i;
				checked++;
			}

		}
	}

	if(!checked)
        throw Retry(string("No available filesystem (") + fileSystems[0] + ")");

	Log::info() << "Least used file system: " << fileSystems[best] << " " << Bytes(free) << " available" << endl;

	return fileSystems[best];
}

static const PathName& roundRobin(const vector<PathName>& fileSystems)
{
	static const char *here = __FUNCTION__;
	static long value = -1;
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

	if(value < 0)
		value = ::getpid();

	value++;
	value %= fileSystems.size();

	Log::info() << "roundRobin selection " << value << " out of " << fileSystems.size() << endl;

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
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " << here << endl;
				Log::error() << "** Exception is ignored" << endl;
				Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << endl;
				continue;
			}

			if(fs.total == 0)
			{
				Log::info() << "roundRobin: cannot get total size of " << fileSystems[i] << endl;
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
				Log::info() << "Round robin file system: " << fileSystems[value] << " " << Bytes(fs.available) << " available" << endl;
				return fileSystems[value];

			}

		}
	}

	Log::info() << "roundRobin reverting to leastUsed" << endl;

	return leastUsed(fileSystems);

}

static const PathName& pureRandom(const vector<PathName>& fileSystems)
{
	static const char *here = __FUNCTION__;
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

	long value = random() % fileSystems.size();

	for(Ordinal j = 0; j < fileSystems.size(); j++)

	{
		Ordinal i = (j + value) % fileSystems.size();
		if(fileSystems[i].available())
		{
			FileSystemSize fs;

			try
			{
				fileSystems[i].fileSystemSize(fs);
			}
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " << here << endl;
				Log::error() << "** Exception is ignored" << endl;
				Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << endl;
				continue;
			}

			if(fs.total == 0)
				return leastUsed(fileSystems);

			long percent = long(((double) (fs.total - fs.available) / fs.total * 100) + 0.5);

			if(percent <= candidate)
			{
				value = i;
				Log::info() << "Pure random file system: " << fileSystems[value] << " " << Bytes(fs.available) << " available" << endl;
				return fileSystems[value];
			}

		}
	}

	return leastUsed(fileSystems);

}

static const PathName& weightedRandom(const vector<PathName>& fileSystems)
{
	static const char *here = __FUNCTION__;
	static Resource<long> candidate("candidateFileSystem", 99);

	ASSERT(fileSystems.size() != 0);

	map<long, long long> scores;
	long long free_space = 0;
	long long scale = 1;

	for(Ordinal i = 0; i < fileSystems.size(); i++)
		if(fileSystems[i].available())
		{

			FileSystemSize fs;

			try
			{
				fileSystems[i].fileSystemSize(fs);
			}
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " << here << endl;
				Log::error() << "** Exception is ignored" << endl;
				Log::error() << "Cannot stat " << fileSystems[i] << Log::syserr << endl;
				continue;
			}

			if(fs.total == 0)
				return leastUsed(fileSystems);

			long percent = long(((double) (fs.total - fs.available) / fs.total * 100) + 0.5);

			if(percent <= candidate)
			{
				scores[i] = fs.available;
				free_space += fs.available;
			}

		}

	if(!scores.size())
		return leastUsed(fileSystems);

	while(free_space > 0x7fffffff)
	{
		free_space /= 2;
		scale *= 2;
	}

	long choice = random() % free_space;
	long value = (*scores.begin()).first;
	long last = 0;

	for(map<long, long long>::iterator j = scores.begin(); j != scores.end(); ++j)
	{
		long s = (*j).second / scale;
#if 0
		Log::info() << "Choice " << choice << " free_space = " << free_space << " last = " << last << " s = " << s << " scale = " << scale << endl;
#endif
		if(choice >= last && choice < last + s)
			value = (*j).first;
		last += s;
	}

	Log::info() << "Weighted random file system: " << fileSystems[value] << " " << Bytes(scores[value]) << " available" << endl;

	return fileSystems[value];
}

const PathName& FileSpace::selectFileSystem(const string& s) const
{

	load();

	if(fileSystems_.size() == 0)
	{
		throw Retry(string("FileSpace [") + name_ + "] is undefined");
	}

	Log::info() << "FileSpace::selectFileSystem is " << s << endl;

	if(s == "roundRobin")
		return roundRobin(fileSystems_);
	if(s == "weightedRandom")
		return weightedRandom(fileSystems_);
	if(s == "pureRandom")
		return pureRandom(fileSystems_);

	return leastUsed(fileSystems_);
}

const PathName& FileSpace::selectFileSystem() const
{
	static Resource<string> s("fileSystemSelection", "leastUsed");
	return selectFileSystem(string(s));
}

bool FileSpace::owns(const PathName& path) const
{
	//Log::info() << "FileSpace::owns " <<  name_ << " " << path << endl;
	bool found = false;
	find(path, found);
	return found;
}

const PathName& FileSpace::sameFileSystem(const PathName& path) const
{
	//Log::info() << "FileSpace::sameFileSystem " <<  name_ << " " << path << endl;

	bool found = false;
	const PathName& f = find(path, found);

	if(found)
	{
		Log::info() << f << " matches " << path << endl;
		return f;
	}
	else
	{
		Log::warning() << "Cannot find matching file system for " << path << endl;
		return selectFileSystem();
	}
}

const PathName& FileSpace::find(const PathName& path, bool& found) const
{
	load();

	PathName m(path.mountPoint());

	//TODO: Cache the mount point...
	for(Ordinal i = 0; i < fileSystems_.size(); i++)
		if(fileSystems_[i].available() && (fileSystems_[i].mountPoint() == m))
		{
			found = true;
			return fileSystems_[i];
		}

	found = false;
	return path;
}

const FileSpace& FileSpace::lookUp(const string& name)
{
    INIT_MUTEX();
	AutoLock<Mutex> lock(mutex);

	Map::iterator j = space.find(name);
	if(j == space.end())
	{
		pair<string, FileSpace*> p(name, new FileSpace(name));
		j = space.insert(p).first;
	}
	return *(*j).second;
}

bool FileSpace::exists(const string& name)
{
	PathName config(string("~/etc/disks/") + name);
	return config.exists();
}


void FileSpace::load() const
{
	FileSpace* self = const_cast<FileSpace*> (this);

	PathName config(string("~/etc/disks/") + name_);
	time_t mod1 = config.lastModified();
	time_t mod2 = ClusterDisks::lastModified(name_);
	time_t modified = ::max(mod1, mod2);

	if(last_ == modified)
		return;

	::srandom(::getpid());

	self->last_ = modified;
	self->fileSystems_.clear();

	//Log::info() << "Loading FileSpace " << name_ << " modified " << TimeStamp(last_) << endl;

	ifstream in(config.localPath());
	if(!in)
		throw CantOpenFile(config);

	vector<string> disks;

	char line[1024];
	while(in >> line)
		if(line[0] != 0 || line[0] != '#')
			disks.push_back(line);

	ClusterDisks::load(name_, disks);

	for(Ordinal i = 0; i < disks.size(); i++)
	{
		PathName path(disks[i]);
		try
		{
			if(path.available())
				path.mkdir();
			self->fileSystems_.push_back(path);
		}
		catch(Exception& e)
		{
			Log::error() << "mkdir(" << path << ") failed : " << e.what() << endl;
		}
	}

}

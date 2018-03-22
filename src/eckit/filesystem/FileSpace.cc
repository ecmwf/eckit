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

#include "eckit/filesystem/FileSpace.h"
#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "eckit/types/Types.h"
#include "eckit/config/Resource.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, FileSpace*> Map;

static Once<Mutex> local_mutex;
static Map space;

FileSpace::FileSpace(const std::string& name) :
	name_(name), last_(0)
{
	AutoLock<Mutex> lock(local_mutex);
	space[name] = this;
	load();
}

FileSpace::~FileSpace()
{
	AutoLock<Mutex> lock(local_mutex);

	space.erase(name_);
}

const PathName& FileSpace::selectFileSystem(const std::string& s) const
{
	load();

	if(fileSystems_.size() == 0)
	{
		throw Retry(std::string("FileSpace [") + name_ + "] is undefined");
	}

    return FileSpaceStrategies::selectFileSystem(fileSystems_, s);
}

const std::string& FileSpace::selectionStrategy() const
{
    if(!strategy_.empty()) return strategy_;

    strategy_ = Resource<std::string>(std::string(name_ + "FileSystemSelection").c_str(), "");

    if(!strategy_.empty()) return strategy_;

    strategy_ = Resource<std::string>("fileSystemSelection", "leastUsed");

    return strategy_;
}

const PathName& FileSpace::selectFileSystem() const
{
    return selectFileSystem(selectionStrategy());
}

bool FileSpace::owns(const PathName& path) const
{
	//Log::info() << "FileSpace::owns " <<  name_ << " " << path << std::endl;
	bool found = false;
	find(path, found);
	return found;
}

const PathName& FileSpace::sameFileSystem(const PathName& path) const
{
	//Log::info() << "FileSpace::sameFileSystem " <<  name_ << " " << path << std::endl;

	bool found = false;
	const PathName& f = find(path, found);

	if(found)
	{
		Log::info() << f << " matches " << path << std::endl;
		return f;
	}
	else
	{
		Log::warning() << "Cannot find matching file system for " << path << std::endl;
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

const FileSpace& FileSpace::lookUp(const std::string& name)
{
	AutoLock<Mutex> lock(local_mutex);

	Map::iterator j = space.find(name);
	if(j == space.end())
	{
        std::pair<std::string, FileSpace*> p(name, new FileSpace(name));
		j = space.insert(p).first;
	}
	return *(*j).second;
}

bool FileSpace::exists(const std::string& name)
{
	PathName config(std::string("~/etc/disks/") + name);
	return config.exists();
}


void FileSpace::load() const
{
	FileSpace* self = const_cast<FileSpace*> (this);

	PathName config(std::string("~/etc/disks/") + name_);
	time_t mod1 = config.lastModified();
	time_t mod2 = ClusterDisks::lastModified(name_);
    time_t modified = std::max(mod1, mod2);

	if(last_ == modified)
		return;

	::srandom(::getpid());

	self->last_ = modified;
	self->fileSystems_.clear();

	//Log::info() << "Loading FileSpace " << name_ << " modified " << TimeStamp(last_) << std::endl;

    std::ifstream in(config.localPath());
	if(!in)
		throw CantOpenFile(config);

	std::vector<std::string> disks;

    char line[1024] = {0,};
    while(in >> line) {
        if(line[0] != 0 || line[0] != '#') {
			disks.push_back(line);
        }
    }

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
		catch(std::exception& e)
		{
			Log::error() << "mkdir(" << path << ") failed : " << e.what() << std::endl;
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

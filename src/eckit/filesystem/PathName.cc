/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/PathName.cc
// Baudouin Raoult - (c) ECMWF Jun 11

#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/Length.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"

#include "eckit/filesystem/BasePathName.h"
#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static BasePathName* make(const std::string& p) {

    if(p.find("marsfs:") == 0) 
        return new BasePathNameT<MarsFSPath>(p);

    /*
    const std::string& node = ClusterDisks::node(p);
    if(node.length())
        return new BasePathNameT<MarsFSPath>(std::string("marsfs://") + node +  p , ext);
    */

    return new BasePathNameT<LocalPathName>(p);
}

PathName::PathName(const char* p)
{
	path_ = make(p);
}

PathName::PathName(const std::string& p)
{
	path_ = make(p);
}

PathName::PathName(const PathName& other):
	path_(other.path_->clone())
{
}

PathName::PathName(const LocalPathName& other):
	path_(new BasePathNameT<LocalPathName>(other))
{
}


PathName::PathName(BasePathName* path):
	path_(path)
{
	ASSERT(path);
}

PathName& PathName::operator=(const PathName& other)
{
	if(this != &other) {
		delete path_;
		path_ = other.path_->clone();
	}
	return *this;
}

PathName& PathName::operator=(const std::string& s)
{
	// TODO: Review me
	*this = PathName(s);
	return *this;
}

PathName& PathName::operator=(const char* s)
{
	// TODO: Review me
	*this = PathName(s);
	return *this;
}

PathName::~PathName()
{
	delete path_;
}


void PathName::print(std::ostream& s) const
{
	s << *path_;
}

const char* PathName::localPath() const
{
	return path_->localPath();
}

PathName PathName::clusterName() const
{
    return PathName(path_->clusterName());
}

Length PathName::size() const
{
	return path_->size();
}

time_t PathName::lastAccess() const
{
	return path_->lastAccess();
}

time_t PathName::lastModified() const
{
	return path_->lastModified();
}

time_t PathName::created() const
{
    return path_->created();
}

bool PathName::isDir() const
{
    return path_->isDir();
}

bool PathName::exists() const
{
	return path_->exists();
}

bool PathName::available() const
{
	return path_->available();
}

void PathName::unlink() const
{
	path_->unlink();
}

void PathName::rmdir() const
{
	path_->rmdir();
}

void PathName::touch() const
{
	path_->touch();
}

void PathName::mkdir(short mode) const
{
	path_->mkdir(mode);
}

void PathName::rename(const PathName& from, const PathName& to)
{
	from.path_->rename(*to.path_);
}

void PathName::link(const PathName& from, const PathName& to)
{
	from.path_->link(*to.path_);
}

void PathName::children(std::vector<PathName>& dirs,std::vector<PathName>& files) const
{
	std::vector<BasePathName*> d;
	std::vector<BasePathName*> f;

	path_->children(d,f);

	for(std::vector<BasePathName*>::iterator j = d.begin(); j != d.end(); ++j)
		dirs.push_back(PathName(*j));

	for(std::vector<BasePathName*>::iterator j = f.begin(); j != f.end(); ++j)
		files.push_back(PathName(*j));
}

void PathName::match(const PathName& path, std::vector<PathName>& result, bool rec)
{
	std::vector<BasePathName*> v;
	path.path_->match(v, rec);
	for(std::vector<BasePathName*>::iterator j = v.begin(); j != v.end(); ++j)
		result.push_back(PathName(*j));
}

bool PathName::operator <(const PathName& other) const
{
	return this->asString() < other.asString();
}

bool PathName::operator >(const PathName& other) const
{
	return this->asString() > other.asString();
}

bool PathName::operator !=(const PathName& other) const
{
	return this->asString() != other.asString();
}

bool PathName::operator ==(const PathName& other) const
{
	return this->asString() == other.asString();
}

bool PathName::operator <=(const PathName& other) const
{
	return this->asString() <= other.asString();
}

bool PathName::operator >=(const PathName& other) const
{
	return this->asString() >= other.asString();
}

PathName PathName::unique(const PathName& path)
{
	return PathName(path.path_->unique());
}

PathName PathName::dirName() const
{
	return PathName(path_->dirName());
}

PathName PathName::orphanName() const
{
	return PathName(path_->orphanName());
}

PathName PathName::checkClusterNode() const
{
	return PathName(path_->checkClusterNode());
}

PathName PathName::baseName(bool ext) const
{
	return PathName(path_->baseName(ext));
}

PathName PathName::fullName() const
{
	return PathName(path_->fullName());
}

void PathName::reserve(const Length& length) const
{
	path_->reserve(length);
}

void PathName::fileSystemSize(FileSystemSize& fs) const
{
	path_->fileSystemSize(fs);
}

PathName PathName::mountPoint() const
{
	return PathName(path_->mountPoint());
}

bool PathName::sameAs(const PathName& other) const
{
	return path_->sameAs(*other.path_);
}

DataHandle* PathName::fileHandle(bool overwrite) const
{
    return path_->fileHandle(overwrite);
}

DataHandle* PathName::partHandle(const OffsetList& o, const LengthList& l) const
{
    return path_->partHandle(o,l);
}

DataHandle* PathName::partHandle(const Offset& o, const Length& l) const
{
    return path_->partHandle(o,l);
}

std::string PathName::asString() const
{
	return path_->asString();
}

PathName operator+(const PathName& p,const std::string& s)
{
	// TODO: delegate that to "path_"
	return PathName(p.asString() + s);
}

PathName operator+(const PathName& p,const char* s)
{
	// TODO: delegate that to "path_"
	return PathName(p.asString() + s);
}

PathName operator+(const PathName& p,char s)
{
	// TODO: delegate that to "path_"
	return PathName(p.asString() + s);
}

PathName operator/(const PathName& p,const std::string& s)
{
	// TODO: delegate that to "path_"
    return PathName(p.asString() + "/" + s);
}

PathName operator/(const PathName& p,const char* s)
{
	// TODO: delegate that to "path_"
	return PathName(p.asString() + "/" + s);
}

PathName operator/(const PathName& p,char s)
{
	// TODO: delegate that to "path_"
	return PathName(p.asString() + "/" + s);
}

void operator<<(Stream& s,const PathName& path)
{
	// TODO: delegate that to "path_"
	s << path.asString();
}

void operator>>(Stream& s,PathName& path)
{
	// TODO: delegate that to "path_"
	std::string p;
	s >> p;
	path = PathName(p);
}


std::string PathName::shorten(const std::string& s) {
	// TODO: Read from ~etc/disk/...


	if(s.find("/locked/") != std::string::npos) return ".../locked/...";
	if(s.find("/transfer/") != std::string::npos) return ".../transfer/...";
	if(s.find("/defrag/") != std::string::npos) return ".../defrag/...";
	if(s.find("/temp/") != std::string::npos) return ".../temp/...";
	if(s.find("/obstmp/") != std::string::npos) return ".../obstmp/...";
	if(s.find("/infrequent/") != std::string::npos) return ".../infrequent/...";
	if(s.find("/prearc/") != std::string::npos) return ".../prearc/...";
	if(s.find("/cache/") != std::string::npos) return ".../cache/...";
	return s.substr(0,10) + "...";
}

const std::string& PathName::node() const
{
	return path_->node();
}

const std::string& PathName::path() const
{
	return path_->path();
}

PathName &PathName::operator /=(const std::string &s)
{
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s);
	return *this;
}

PathName &PathName::operator /=(const char *s)
{
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s );
	return *this;
}

PathName &PathName::operator /=(char s)
{
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s);
	return *this;
}
PathName &PathName::operator +=(const std::string &s)
{
    // TODO: Review me
    *this = PathName(this->asString() + s);
	return *this;
}

PathName &PathName::operator +=(const char *s)
{
    // TODO: Review me
    *this = PathName(this->asString() + s );
	return *this;
}

PathName &PathName::operator +=(char s)
{
    // TODO: Review me
    *this = PathName(this->asString() + s);
	return *this;
}

//-----------------------------------------------------------------------------

} // namespace eckit

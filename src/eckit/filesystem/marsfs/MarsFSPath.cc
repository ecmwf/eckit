/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/Length.h"
#include "eckit/filesystem/marsfs/MarsFSClient.h"
#include "eckit/io/MarsFSHandle.h"
#include "eckit/io/MarsFSPartHandle.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/thread/Mutex.h"
#include "eckit/io/cluster/NodeInfo.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

MarsFSPath::MarsFSPath(const std::string& path)
{
    std::string p = path;
    //node_ = NodeInfo::thisNode().node();

    ASSERT(p.find("marsfs:") == 0);
    {
        p = p.substr(7);
    }

    ASSERT(p.find("//") == 0);
    {
        p        = p.substr(2);
        size_t n = p.find("/");
        ASSERT(n != std::string::npos);

        node_ = p.substr(0,n);
        p     = p.substr(n);


    }
    path_ = p;

    PANIC(path_.find("marsfs:") != std::string::npos);
    PANIC(node_.find("marsfs:") != std::string::npos);

}

MarsFSPath::MarsFSPath(const std::string& node, const std::string& path):
    node_(node),
    path_(path)
{
    PANIC(node_.find("marsfs:") != std::string::npos);
    PANIC(path_.find("marsfs:") != std::string::npos);
}

MarsFSPath::MarsFSPath(const MarsFSPath& other):
    node_(other.node_),
    path_(other.path_)
{
    PANIC(node_.find("marsfs:") != std::string::npos);
    PANIC(path_.find("marsfs:") != std::string::npos);
}

MarsFSPath& MarsFSPath::operator=(const MarsFSPath& other)
{
    node_ = other.node_;
    path_ = other.path_;
    PANIC(node_.find("marsfs:") != std::string::npos);
    PANIC(path_.find("marsfs:") != std::string::npos);
    return *this;
}

MarsFSPath::MarsFSPath(Stream& s)
{
    s >> node_;
    s >> path_;
}

MarsFSPath::~MarsFSPath()
{
}


void MarsFSPath::print(std::ostream& s) const
{
    s << "marsfs://" << node_ << path_;
}

void operator<<(Stream& s, MarsFSPath const& path)
{
    s << path.node_;
    s << path.path_;
}

bool MarsFSPath::isLocal() const
{
    return node_ == NodeInfo::thisNode().node();
}

MarsFSPath::operator std::string() const
{
    return std::string("marsfs://") + node_ + path_;
    //§return path_;
}


MarsFSPath MarsFSPath::dirName() const
{
    return MarsFSPath(node(),MarsFSClient(*this).dirName(path_));
}

MarsFSPath MarsFSPath::fullName() const
{
    return MarsFSPath(node(),MarsFSClient(*this).fullName(path_));
}

MarsFSPath MarsFSPath::baseName(bool ext) const
{
    return MarsFSPath(node(),MarsFSClient(*this).baseName(path_, ext));
}

void MarsFSPath::touch() const
{
    MarsFSClient(*this).touch(path_);
}

Length MarsFSPath::size() const
{
    return MarsFSClient(*this).size(path_);
}

bool MarsFSPath::sameAs(const MarsFSPath& other) const
{
    if(node() != other.node())
        return false;
    return MarsFSClient(*this).sameAs(path_, other.path_);
}

MarsFSPath MarsFSPath::unique(const MarsFSPath& path)
{
    return MarsFSPath(path.node(),MarsFSClient(path).unique(path.path_));
}

void MarsFSPath::reserve(const Length&) const
{
    NOTIMP;
}

void MarsFSPath::mkdir(short mode) const
{
    MarsFSClient(*this).mkdir(path_, mode);
}

void MarsFSPath::fileSystemSize(FileSystemSize& fs) const
{
    MarsFSClient(*this).fileSystemSize(path_, fs);
}


time_t MarsFSPath::created() const
{
    return MarsFSClient(*this).created(path_);
}

bool MarsFSPath::isDir() const
{
    throw NotImplemented(Here());
}

time_t MarsFSPath::lastAccess() const
{
    return MarsFSClient(*this).lastAccess(path_);
}

time_t MarsFSPath::lastModified() const
{
    return MarsFSClient(*this).lastModified(path_);
}

bool MarsFSPath::exists() const
{
    return MarsFSClient(*this).exists(path_);
}

bool MarsFSPath::available() const
{
    return ClusterNodes::available("marsfs", node_);
}

void MarsFSPath::unlink() const
{
    MarsFSClient(*this).unlink(path_);
}

void MarsFSPath::rmdir() const
{
    MarsFSClient(*this).rmdir(path_);
}

void MarsFSPath::syncParentDirectory() const
{
    MarsFSClient(*this).syncParentDirectory(path_);
}

const char* MarsFSPath::localPath() const
{
    throw SeriousBug(std::string("Attempting to access ") + std::string(*this) + " locally");
}


void MarsFSPath::children(std::vector<MarsFSPath>& dirs,std::vector<MarsFSPath>& files) const
{
    std::vector<std::string> d;
    std::vector<std::string> f;

    MarsFSClient(*this).children(path_, d, f);

    dirs.clear();
    for(std::vector<std::string>::iterator j = d.begin(); j != d.end(); ++j)
        dirs.push_back(MarsFSPath(node(), *j));

    files.clear();
    for(std::vector<std::string>::iterator j = f.begin(); j != f.end(); ++j)
        files.push_back(MarsFSPath(node(), *j));
}

void MarsFSPath::match(const MarsFSPath& path,std::vector<MarsFSPath>& result,bool recurse)
{
    std::vector<std::string> r;
    MarsFSClient(path).match(path.path_, r, recurse);
    result.clear();
    for(std::vector<std::string>::iterator j = r.begin(); j != r.end(); ++j)
        result.push_back(MarsFSPath(path.node(), *j));
}

void MarsFSPath::link(const MarsFSPath& from,const MarsFSPath& to)
{
    ASSERT(from.node_ == to.node_);
    MarsFSClient(from).link(from.path_, to.path_);
}

void MarsFSPath::rename(const MarsFSPath& from,const MarsFSPath& to)
{
    ASSERT(from.node_ == to.node_);
    MarsFSClient(from).rename(from.path_, to.path_);
}

DataHandle* MarsFSPath::fileHandle(bool overwrite) const
{
    return new MarsFSHandle(*this, overwrite);
    //return new MoverHandle(new MarsFSHandle(*this, overwrite));
}

DataHandle* MarsFSPath::partHandle(const OffsetList& o, const LengthList& l) const
{
    return new MarsFSPartHandle(*this, o, l);
}

DataHandle* MarsFSPath::partHandle(const Offset& o, const Length& l) const
{
    return new MarsFSPartHandle(*this, o, l);
}

MarsFSPath MarsFSPath::mountPoint() const
{
    return MarsFSPath(node_, MarsFSClient(*this).mountPoint(path_));
}


MarsFSPath MarsFSPath::realName() const
{
    NOTIMP;
    // return MarsFSPath(node_, MarsFSClient(*this).realName(path_));
}

MarsFSPath MarsFSPath::orphanName() const
{

    std::ostringstream os;
    os << mountPoint()  << "/orphans/";

    const char *q = path_.c_str();
    while(*q) {
        if(*q == '/')  os << '_';
        else os << *q;
        q++;
    }

    return os.str();

}

std::string MarsFSPath::clusterName() const
{
    return std::string(*this);
}

std::string MarsFSPath::extension() const {
    NOTIMP;
}

BasePathName* MarsFSPath::checkClusterNode() const
{
    try
    {
        std::string n = ClusterDisks::node(path_);
        ASSERT(n != NodeInfo::thisNode().node()); // TODO: code mo, if a remote file becomes local
        if(n != node_) {
//            Log::warning() << *this << " is now on node [" << n << "]" << std::endl;
        }
        return new BasePathNameT<MarsFSPath>(MarsFSPath(n, path_));
    }
    catch(std::exception& e)
    {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is handled" << std::endl;
        return new BasePathNameT<MarsFSPath>(MarsFSPath(node_, path_));
    }
}

//-----------------------------------------------------------------------------

} // namespace eckit


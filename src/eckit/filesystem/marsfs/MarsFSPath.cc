/*
 * (C) Copyright 1996-2013 ECMWF.
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
#include "eckit/compat/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

MarsFSPath::MarsFSPath(const string& path)
{
    string p = path;
    //node_ = NodeInfo::thisNode().node();

    ASSERT(p.find("marsfs:") == 0);
    {
        p = p.substr(7);
    }

    ASSERT(p.find("//") == 0);
    {
        p        = p.substr(2);
        size_t n = p.find("/");
        ASSERT(n != string::npos);

        node_ = p.substr(0,n);
        p     = p.substr(n);


    }
    path_ = p;

    PANIC(path_.find("marsfs:") != string::npos);
    PANIC(node_.find("marsfs:") != string::npos);

}

MarsFSPath::MarsFSPath(const string& node, const string& path):
    node_(node),
    path_(path)
{
    PANIC(node_.find("marsfs:") != string::npos);
    PANIC(path_.find("marsfs:") != string::npos);
}

MarsFSPath::MarsFSPath(const MarsFSPath& other):
    node_(other.node_),
    path_(other.path_)
{
    PANIC(node_.find("marsfs:") != string::npos);
    PANIC(path_.find("marsfs:") != string::npos);
}

MarsFSPath& MarsFSPath::operator=(const MarsFSPath& other)
{
    node_ = other.node_;
    path_ = other.path_;
    PANIC(node_.find("marsfs:") != string::npos);
    PANIC(path_.find("marsfs:") != string::npos);
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


void MarsFSPath::print(ostream& s) const
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

MarsFSPath::operator string() const
{
    return string("marsfs://") + node_ + path_;
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

const char* MarsFSPath::localPath() const
{
    throw SeriousBug(string("Attempting to access ") + string(*this) + " locally");
}


void MarsFSPath::children(vector<MarsFSPath>& dirs,vector<MarsFSPath>& files) const
{
    vector<string> d;
    vector<string> f;

    MarsFSClient(*this).children(path_, d, f);

    dirs.clear();
    for(vector<string>::iterator j = d.begin(); j != d.end(); ++j)
        dirs.push_back(MarsFSPath(node(), *j));

    files.clear();
    for(vector<string>::iterator j = f.begin(); j != f.end(); ++j)
        files.push_back(MarsFSPath(node(), *j));
}

void MarsFSPath::match(const MarsFSPath& path,vector<MarsFSPath>& result,bool recurse)
{
    vector<string> r;
    MarsFSClient(path).match(path.path_, r, recurse);
    result.clear();
    for(vector<string>::iterator j = r.begin(); j != r.end(); ++j)
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

MarsFSPath MarsFSPath::orphanName() const
{

    StrStream os;
    os << mountPoint()  << "/orphans/";

    const char *q = path_.c_str();
    while(*q) {
        if(*q == '/')  os << '_';
        else os << *q;
        q++;
    }

    os << StrStream::ends;

    string s(os);
    return s;

}

string MarsFSPath::clusterName() const
{
    return string(*this);
}

BasePathName* MarsFSPath::checkClusterNode() const
{
    try 
    {
        string n = ClusterDisks::node(path_);
        ASSERT(n != NodeInfo::thisNode().node()); // TODO: code mo, if a remote file becomes local
        if(n != node_) {
//            Log::warning() << *this << " is now on node [" << n << "]" << endl;
        }
        return new BasePathNameT<MarsFSPath>(MarsFSPath(n, path_));
    }
    catch(Exception& e)
    {
        Log::error() << "** " << e.what() << " Caught in " << Here() <<  endl;
        Log::error() << "** Exception is handled" << endl;
        return new BasePathNameT<MarsFSPath>(MarsFSPath(node_, path_));
    }
}

//-----------------------------------------------------------------------------

} // namespace eckit


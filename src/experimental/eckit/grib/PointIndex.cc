/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/AutoLock.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"
#include "eckit/io/StdFile.h"
#include "eckit/os/Stat.h"
#include "eckit/log/Timer.h"

#include "eckit/grib/Grib.h"
#include "eckit/grib/PointIndex.h"
#include "eckit/grib/GribHandle.h"

using namespace eckit;
using namespace eckit::grib;

//------------------------------------------------------------------------------------------------------

static Once<Mutex>  local_mutex;

static std::set<std::string> done_;
static std::map<PathName,PointIndex*> cache_;

//------------------------------------------------------------------------------------------------------

std::string PointIndex::cache(grib_handle* gh)
{
	ASSERT(gh);
	GribHandle h(*gh);

    size_t v;
    int ret;
    double lat, lon, value;

	std::string md5 = h.geographyHash();

    AutoLock<Mutex> lock(local_mutex);
    if(done_.find(md5) != done_.end()) {
        return md5;
    }

    eckit::PathName path(std::string("~/etc/pointdb/") + md5 + ".kdtree");
    if(path.exists()) {
        done_.insert(md5);
        return md5;
    }
    PathName("~/etc/pointdb/").mkdir();
    // radiusInMetres

	v = h.getDataValuesSize();
	//cout << "values: " << v << endl;

    std::vector<Tree::Value> p;
    p.reserve(v);
	grib_iterator *i = grib_iterator_new(gh, 0, &ret);
    size_t j = 0;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {


        while(lon < 0)    lon += 360;
        while(lon >= 360) lon -= 360;

        p.push_back(Tree::Value(Point(lat,lon),j));
        //   p.push_back(Point(lat,lon-360,j));
        //  p.push_back(Point(lat,lon+360,j));
        j++;
    }
    grib_iterator_delete(i);


    PathName tmp(std::string("~/etc/pointdb/") + md5 + ".tmp");
    tmp.unlink();

    Tree* tree = new Tree(tmp, v, 0);
    tree->build(p.begin(), p.end());

    PathName dump(std::string("~/etc/pointdb/") + md5 + ".dump");
    StdFile f(dump,"w");
	grib_dump_content(gh, f, "debug", 0,0);

    PathName grib(std::string("~/etc/pointdb/") + md5 + ".grib");
	ASSERT(grib_write_message(gh,grib.localPath(),"w") == 0);

    PathName::rename(tmp, path);

    cache_[md5] = new PointIndex(path, tree);
    done_.insert(md5);
    return md5;
}

PointIndex& PointIndex::lookUp(const std::string& md5)
{
    AutoLock<Mutex> lock(local_mutex);
    std::map<PathName,PointIndex*>::iterator k = cache_.find(md5);
    if(k == cache_.end()) {
        eckit::PathName path(std::string("~/etc/pointdb/") + md5 + ".kdtree");

        if(!path.exists())
        {
            Log::warning() << path << " does not exists" << std::endl;
            PathName grib(std::string("~/etc/pointdb/") + md5 + ".grib");
            if(grib.exists()) {
                Log::warning() << "Rebuilding index from " << grib << std::endl;
                StdFile f(grib,"r");
                int err=0;
                grib_handle* h = grib_handle_new_from_file(0,f,&err);
                ASSERT(h != 0 && err == 0);
                ASSERT(cache(h) == md5);
                grib_handle_delete(h);
            }
        }

        Log::warning() << "Loading " << path << std::endl;
        PointIndex* p = new PointIndex(path);
        cache_[md5] = p;
        return *p;
    }
    return *(*k).second;
}

PointIndex::PointIndex(const PathName& path, PointIndex::Tree* tree):
    path_(path),
    tree_(tree)
{
    if(!tree) {
        ASSERT(path.exists());
        tree_.reset(new Tree(path,0,0));
    }
}

PointIndex::~PointIndex()
{
    // TODO
}

PointIndex::NodeInfo PointIndex::nearestNeighbour(double lat, double lon)
{
    Point p(lat,lon);

    std::map<Point,NodeInfo>::iterator k;

    {
        AutoLock<Mutex> lock(mutex_);

        k = last_.find(p);
        if(k != last_.end())
            return  (*k).second;
    }

    NodeInfo n(0,0,0);

    {
        Timer timer("Find nearest");
        n = tree_->nearestNeighbour(p);
    }

    {
        AutoLock<Mutex> lock(mutex_);

        if(last_.size() >= 4096)
            last_.clear();

        last_[p] = n;
    }

    return n;
}

//------------------------------------------------------------------------------------------------------


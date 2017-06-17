/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathExpander.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/LocalPathName.h"

#include "eckit/parser/StringTools.h"

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once  = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex = 0;

static void init() {
    local_mutex = new eckit::Mutex();
}

typedef std::map<std::string, PathExpander*> PathExpanderMap;

struct PathExpanderRegistry {

    static PathExpanderRegistry& instance() {
        static PathExpanderRegistry reg;
        return reg;
    }

    PathExpanderMap map_;

    PathExpanderMap& map() { return map_; }

    const PathExpander& lookup(const std::string& name) {

        pthread_once(&once, init);
        eckit::AutoLock<eckit::Mutex> lock(local_mutex);

        PathExpanderMap& m = PathExpanderRegistry::instance().map();

        PathExpanderMap::const_iterator j = m.find(name);

        // eckit::Log::info() << "Looking for PathExpander '" << name << "'" << std::endl;

        if (j == m.end()) {
            eckit::Log::error() << "No PathExpander found with name '" << name << "'" << std::endl;
            eckit::Log::error() << "Registered libraries are:" << std::endl;
            for (j = m.begin() ; j != m.end() ; ++j)
                eckit::Log::error() << "   " << (*j).first << std::endl;
            throw eckit::UserError(std::string("No PathExpander found with name ") + name);
        }

        ASSERT(j->second);

        return *(j->second);
    }

};

//----------------------------------------------------------------------------------------------------------------------


std::string PathExpander::expand(const std::string& key, const std::string& path)
{
    return PathExpanderRegistry::instance().lookup(key).expand(path);
}


PathExpander::PathExpander(const std::string& name):
        name_(name)
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    PathExpanderMap& m = PathExpanderRegistry::instance().map();

    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

PathExpander::~PathExpander() {
}

void PathExpander::print(std::ostream& s) const
{
    s << "PathExpander[" << name_ << "]";
}


//----------------------------------------------------------------------------------------------------------------------

class CWDFS : public PathExpander {
public:

    CWDFS(const std::string& name) : PathExpander(name) {}

    virtual std::string expand(const std::string& path) const {

        LocalPathName mnt = LocalPathName::cwd().mountPoint();

        StringDict m;
        m["CWDFS"] = std::string(mnt);

        std::string newpath = StringTools::substitute(path, m);

        std::cout << "Path expansion " << path << " --> " << newpath << std::endl;

        return newpath;
    }

};

static CWDFS cwdfs("CWDFS");

//----------------------------------------------------------------------------------------------------------------------

class CWD : public PathExpander {
public:

    CWD(const std::string& name) : PathExpander(name) {}

    virtual std::string expand(const std::string& path) const {

        LocalPathName mnt = LocalPathName::cwd();

        StringDict m;
        m["CWD"] = std::string(mnt);

        std::string newpath = StringTools::substitute(path, m);

        std::cout << "Path expansion " << path << " --> " << newpath << std::endl;

        return newpath;
    }

};

static CWD cwd("CWD");

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

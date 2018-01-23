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

        if (j == m.end()) {
            std::ostringstream msg;
            msg << "No PathExpander found with name '" << name << "'. Registered path expand handlers are:";
            for (j = m.begin() ; j != m.end() ; ++j)
                msg << " '" << (*j).first << "'";
            throw eckit::UserError(msg.str());
        }

        ASSERT(j->second);

        return *(j->second);
    }

};

//----------------------------------------------------------------------------------------------------------------------


std::string PathExpander::expand(const std::string& path)
{
    StringDict vars;

    StringList vl = StringTools::listVariables(path);
    for (StringList::const_iterator it = vl.begin(); it != vl.end(); ++it) {

        const std::string& incurly = *it;

        if(incurly.empty()) throw BadValue("PathExpander received empty key");

        size_t pos = incurly.find_first_of(":");
        std::string key = incurly.substr(0, pos);

        ECKIT_DEBUG_VAR(key);

        PathExpanderRegistry::instance().lookup(key).expand(incurly, path, vars);
    }

    std::string newpath = StringTools::substitute(path, vars);

    std::cerr << "Path expansion " << path << " --> " << newpath << std::endl;

    return newpath;
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

class ENVVAR : public PathExpander {
public:

    ENVVAR(const std::string& name) : PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {

        size_t pos = var.find_first_of(":");
        std::string key = var.substr(0, pos);
        ECKIT_DEBUG_VAR(key);

        ASSERT(key == "ENVVAR");

        if(pos == std::string::npos || pos+1 == std::string::npos) {
            throw eckit::BadValue(std::string("ENVVAR passed but no variable defined: ") + var, Here());
        }

        std::string param = var.substr(pos+1, std::string::npos);
        ECKIT_DEBUG_VAR(param);

        std::string envvar;
        char* e = ::getenv(param.c_str());
        if(e) envvar = e;

        ECKIT_DEBUG_VAR(envvar);

        vars[var] = envvar;
    }

};

static ENVVAR envvar("ENVVAR");

//----------------------------------------------------------------------------------------------------------------------

class CWDFS : public PathExpander {
public:

    CWDFS(const std::string& name) : PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {

        LocalPathName mnt = LocalPathName::cwd().mountPoint();

        vars["CWDFS"] = std::string(mnt);
    }

};

static CWDFS cwdfs("CWDFS");

//----------------------------------------------------------------------------------------------------------------------

class CWD : public PathExpander {
public:

    CWD(const std::string& name) : PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {

        LocalPathName mnt = LocalPathName::cwd();

        vars["CWD"] = std::string(mnt);
    }

};

static CWD cwd("CWD");

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <sstream>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathExpander.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/memory/Zero.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/StringTools.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, PathExpander*> PathExpanderMap;

struct PathExpanderRegistry {
    eckit::Mutex mutex_;

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }

    static PathExpanderRegistry& instance() {
        static PathExpanderRegistry reg;
        return reg;
    }

    PathExpanderMap map_;

    PathExpanderMap& map() { return map_; }

    const PathExpander& lookup(const std::string& name) {
        PathExpanderMap& m = PathExpanderRegistry::instance().map();

        PathExpanderMap::const_iterator j = m.find(name);

        if (j == m.end()) {
            std::ostringstream msg;
            msg << "No PathExpander found with name '" << name << "'. Registered path expand handlers are:";
            for (j = m.begin(); j != m.end(); ++j)
                msg << " '" << (*j).first << "'";
            throw eckit::UserError(msg.str());
        }

        ASSERT(j->second);

        return *(j->second);
    }
};

//----------------------------------------------------------------------------------------------------------------------


std::string PathExpander::expand(const std::string& path) {
    StringDict vars;

    StringList vl = StringTools::listVariables(path);
    for (StringList::const_iterator it = vl.begin(); it != vl.end(); ++it) {
        const std::string& incurly = *it;

        if (incurly.empty())
            throw BadValue("PathExpander received empty key");

        size_t pos      = incurly.find_first_of("?");
        std::string key = incurly.substr(0, pos);

        eckit::AutoLock<PathExpanderRegistry> locker(PathExpanderRegistry::instance());

        PathExpanderRegistry::instance().lookup(key).expand(incurly, path, vars);
    }

    std::string newpath = StringTools::substitute(path, vars);

    Log::debug<LibEcKit>() << "Path expansion " << path << " --> " << newpath << std::endl;

    return newpath;
}

PathExpander::PathExpander(const std::string& name) :
    name_(name) {
    eckit::AutoLock<PathExpanderRegistry> locker(PathExpanderRegistry::instance());

    PathExpanderMap& m = PathExpanderRegistry::instance().map();

    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

PathExpander::~PathExpander() {}

void PathExpander::print(std::ostream& s) const {
    s << "PathExpander[" << name_ << "]";
}

std::ostream& operator<<(std::ostream& s, const PathExpander& p) {
    p.print(s);
    return s;
}


//----------------------------------------------------------------------------------------------------------------------

class ENVVAR : public PathExpander {
public:
    ENVVAR(const std::string& name) :
        PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {
        size_t pos      = var.find_first_of("?");
        std::string key = var.substr(0, pos);

        ASSERT(key == "ENVVAR");

        if (pos == std::string::npos || pos + 1 == std::string::npos) {
            throw eckit::BadValue(std::string("PathExpander ENVVAR passed but no variable defined: ") + var, Here());
        }

        std::string param = var.substr(pos + 1, std::string::npos);

        char* e = ::getenv(param.c_str());

        if (!e) {
            throw eckit::BadValue(std::string("PathExpander ENVVAR passed undefined environment variable: ") + param,
                                  Here());
        }

        std::string envvar(e);

        vars[var] = envvar;
    }
};

static ENVVAR envvar("ENVVAR");

//----------------------------------------------------------------------------------------------------------------------

class FILE : public PathExpander {
public:
    FILE(const std::string& name) :
        PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {
        size_t pos      = var.find_first_of("?");
        std::string key = var.substr(0, pos);

        ASSERT(key == "FILE");

        if (pos == std::string::npos || pos + 1 == std::string::npos) {
            throw eckit::BadValue(std::string("PathExpander FILE passed but no file defined: ") + var, Here());
        }

        PathName p = var.substr(pos + 1, std::string::npos);

        std::ifstream in(p.localPath());
        if (!in) {
            eckit::Log::error() << "PathExpander read error in " << p << " -- " << eckit::Log::syserr << std::endl;
            return;
        }

        char line[4 * 1024] = {};
        in.getline(line, sizeof(line));

        std::string value(line);

        vars[var] = value;
    }
};

static FILE file("FILE");

//----------------------------------------------------------------------------------------------------------------------

class CWDFS : public PathExpander {
public:
    CWDFS(const std::string& name) :
        PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {
        LocalPathName mnt = LocalPathName::cwd().mountPoint();

        vars["CWDFS"] = std::string(mnt);
    }
};

static CWDFS cwdfs("CWDFS");

//----------------------------------------------------------------------------------------------------------------------

class CWD : public PathExpander {
public:
    CWD(const std::string& name) :
        PathExpander(name) {}

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const {
        LocalPathName mnt = LocalPathName::cwd();

        vars["CWD"] = std::string(mnt);
    }
};

static CWD cwd("CWD");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

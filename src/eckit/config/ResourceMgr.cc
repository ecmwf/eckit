/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>  // for strlen
#include <fstream>

#include "eckit/config/ResourceMgr.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceMgr& ResourceMgr::instance() {
    static ResourceMgr theinstance;
    return theinstance;
}

bool ResourceMgr::lookUp(const std::string& s1, const std::string& s2, const std::string& s3, std::string& v) {
    return ResourceMgr::instance().doLookUp(s1, s2, s3, v);
}

void ResourceMgr::reset() {
    AutoLock<Mutex> lock(mutex_);
    resmap_.clear();
    inited_ = false;
}

// This has to be redone

static const char* skip_spaces(const char* p) {
    while (*p && isspace(*p))
        p++;
    return p;
}

bool ResourceMgr::parse(const char* p) {
    p = skip_spaces(p);

    if (*p == 0 || *p == '#')
        return true;  // skip comments

    std::string s[3];
    int n = 0;

    for (;;) {
        const char* q = p;

        p = skip_spaces(p);
        while (*p && *p != ':' && *p != '.' && !isspace(*p))
            p++;

        int len = p - q;
        p       = skip_spaces(p);

        s[n] = q;
        s[n].resize(len);
        n++;

        if (n == 3 || *p != '.')
            break;
        p++;
    }
    if (*p != ':')
        return false;
    else {
        switch (n) {
            case 1:
                s[2] = s[0];
                s[0] = "";
                break;
            case 2:
                s[2] = s[1];
                s[1] = s[0];
                s[0] = "";
                break;
            case 3:
                break;
        }

        p++;
        p = skip_spaces(p);

        // Remove trailing blanks
        int l = ::strlen(p) - 1;
        while (l >= 0 && isspace(p[l]))
            l--;


        ResourceQualifier x(s[0], s[1], s[2]);

        std::string t = std::string(p, l + 1);
        resmap_[x]    = t;
    }
    return true;
}

void ResourceMgr::readConfigFile(const LocalPathName& file) {

    // Log::info() << "ResourceMgr::readConfigFile(" << file << ")" << std::endl;

    int cnt = 0;

    std::ifstream in(file.localPath());
    char line[1024];

    while (in.getline(line, sizeof(line))) {
        cnt++;
        if (!parse(line)) {
            Log::warning() << "Invalid line, file " << file << " line " << cnt << " = " << line << std::endl;
        }
    }
}

void ResourceMgr::set(const std::string& name, const std::string& value) {
    AutoLock<Mutex> lock(mutex_);

    std::string s = name + ": " + value;
    if (!parse(s.c_str()))
        Log::warning() << "Failed to parse " << s << std::endl;
}

bool ResourceMgr::doLookUp(const std::string& kind, const std::string& owner, const std::string& name,
                           std::string& result) {
    AutoLock<Mutex> lock(mutex_);

    if (!inited_) {
        inited_ = true;
        readConfigFile("~/etc/config/general");
        readConfigFile("~/etc/config/local");
        readConfigFile(std::string("~/etc/config/") + Main::instance().name());
        readConfigFile(std::string("~/etc/config/") + Main::instance().name() + ".local");
    }

    ResMap::iterator i = resmap_.find(ResourceQualifier(kind, owner, name));

    if (i != resmap_.end()) {
        result = (*i).second;
        return true;
    }

    i = resmap_.find(ResourceQualifier("", owner, name));

    if (i != resmap_.end()) {
        result = (*i).second;
        return true;
    }

    i = resmap_.find(ResourceQualifier("", "", name));

    if (i != resmap_.end()) {
        result = (*i).second;
        return true;
    }

    return false;
}

bool ResourceMgr::registCmdArgOptions(const std::string&) {
    AutoLock<Mutex> lock(mutex_);
    NOTIMP;
}

ResourceMgr::ResourceMgr() :
    resmap_(), resoptions_(), inited_(false) {}

//----------------------------------------------------------------------------------------------------------------------

ResourceQualifier::ResourceQualifier(const std::string& kind, const std::string& owner, const std::string& name) :
    kind_(kind), owner_(owner), name_(name) {}

ResourceQualifier::ResourceQualifier(const ResourceQualifier& other) {
    kind_  = other.kind_;
    owner_ = other.owner_;
    name_  = other.name_;
}

int ResourceQualifier::operator<(const ResourceQualifier& other) const {
    // to be rewritten
    char buf1[1024];
    char buf2[1024];

    sprintf(buf1, "%s.%s.%s", owner_.c_str(), kind_.c_str(), name_.c_str());
    sprintf(buf2, "%s.%s.%s", other.owner_.c_str(), other.kind_.c_str(), other.name_.c_str());

    return strcmp(buf1, buf2) < 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

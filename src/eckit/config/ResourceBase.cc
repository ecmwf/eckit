/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/runtime/Main.h"

namespace eckit {

ResourceBase::ResourceBase(Configurable* owner, const std::string& str) :
    owner_(owner), inited_(false) {
    if (owner_)
        owner_->add(this);

    const char* p = str.c_str();

    while (*p) {
        std::string* s = &name_;
        char x         = *p;
        int len        = 0;

        switch (x) {
            case '$':
                s = &environment_;
                break;
            case '-':
                s = &options_;
                break;
        }

        *s = p;

        while (*p && *p != ';') {
            len++;
            p++;
        }

        s->resize(len);

        if (*p)
            p++;
    }
}

ResourceBase::~ResourceBase() {
    if (owner_)
        owner_->remove(this);
}

bool ResourceBase::setFromConfigFile() {
    bool found = false;

    std::string s;

    if (owner_)
        found = ResourceMgr::lookUp(owner_->kind(), owner_->name(), name_, s);
    else
        found = ResourceMgr::lookUp("", "", name_, s);

    if (found) {
        setValue(s);
    }

    return found;
}

void ResourceBase::init() {
    if (inited_)
        return;

    // First look in config file
    // First look for an option on the command line

    if (options_ != "") {
        for (int i = 1; i < Main::instance().argc(); i++)
            if (options_ == Main::instance().argv(i)) {
                if (i + 1 == Main::instance().argc() || Main::instance().argv(i + 1)[0] == '-')
                    setValue("true");
                else
                    setValue(Main::instance().argv(i + 1));
                inited_ = true;
                return;
            }
    }

    // Then look for an environment variable

    if (environment_ != "") {
        const char* p = ::getenv(environment_.c_str() + 1);
        if (p) {
            setValue(p);
            inited_ = true;
            return;
        }
    }

    // Otherwise look in the config file

    if (name_ != "") {
        if (setFromConfigFile()) {
            inited_ = true;
            return;
        }
    }

    // Else use default. This is done in Resource

    inited_ = true;
}

std::string ResourceBase::name() const {
    if (owner_)
        return owner_->kind() + '.' + owner_->name() + '.' + name_;
    else
        return name_;
}

void ResourceBase::dump(std::ostream& s) const {

    // need const_cast here
    ((ResourceBase*)this)->init();

    s << "# " << name_ << ":" << std::endl;

    if (options_ != "")
        s << "#   command line option  " << options_ << std::endl;
    if (environment_ != "") {
        s << "#   environment variable " << environment_ << " ";
        const char* p = getenv(environment_.c_str() + 1);
        if (p)
            s << "(defined as " << p << ")";
        else
            s << "(undefined)";
        s << std::endl;
    }

    s << name() << " : " << getValue();

    s << std::endl;
}

}  // namespace eckit

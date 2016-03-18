/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @author Simon Smart
/// @date March 2016


#include <iostream>
#include <map>

#include "eckit/option/CmdArgs.h"
#include "eckit/option/Option.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/runtime/Context.h"


namespace eckit {
namespace option {


//----------------------------------------------------------------------------------------------------------------------


CmdArgs::CmdArgs(usage_proc usage, int args_count, bool throw_on_error) {
    init(usage, args_count, throw_on_error);
}

CmdArgs::CmdArgs(usage_proc usage, int args_count,  std::vector< option::Option *> &options, bool throw_on_error) {
    std::swap(options_, options); // Take ownership so it can be destroyed
    init(usage, args_count, throw_on_error);
}

void CmdArgs::init(usage_proc usage, int args_count, bool throw_on_error)  {
    Context &ctx = Context::instance();
    const std::string &tool = ctx.runName();
    size_t argc = ctx.argc();
    bool error = false;

    std::map<std::string, const option::Option *> opts;

    for (std::vector<option::Option *>::const_iterator j = options_.begin(); j != options_.end(); ++j) {
        if ((*j)->active()) {
            ASSERT(opts.find((*j)->name()) == opts.end());
            opts[(*j)->name()] = *j;
            keys_.insert((*j)->name());
        }
    }

    Tokenizer parse("=");
    for (size_t i = 1; i < argc; i++) {

        std::string a = ctx.argv(i);
        if (a.size() > 2 && a[0] == '-' && a[1] == '-') {
            std::vector<std::string> v;
            parse(a.substr(2), v);
            ASSERT(v.size() <= 2);

            std::map<std::string, const option::Option *>::const_iterator j = opts.find(v[0]);
            if (j != opts.end()) {
                if (v.size() == 1) {
                    (*j).second->set(*this);
                } else {
                    (*j).second->set(v[1], *this);
                }
            } else {
                Log::info() << "Invalid option --" << v[0] << std::endl;
                error = true;
            }
        } else {
            args_.push_back(a);
        }
    }


    if (args_count >= 0) {
        if (args_.size() != size_t(args_count)) {
            Log::info() << "Invalid argument count: expected " << args_count << ", got: " << args_.size() << std::endl;
            error = true;
        }
    }

    if (error) {
        usage(tool);
        if (options_.size()) {
            Log::info() << std::endl;
            Log::info() << "Options are:" << std::endl;
            Log::info() << "===========:" << std::endl ;
            for (std::vector<option::Option *>::const_iterator j = options_.begin(); j != options_.end(); ++j) {
                Log::info() << *(*j) << std::endl << std::endl;
            }
            Log::info() << std::endl;
        }

        if (throw_on_error)
            throw UserError("An error occurred in argument parsing", Here());
        else
            ::exit(1);

    }
}


CmdArgs::~CmdArgs() {
    for (std::vector<option::Option*>::iterator j = options_.begin(); j  != options_.end(); ++j) {
        delete (*j);
    }
}


void CmdArgs::print(std::ostream &out) const {
    out << "CmdArgs[";
    LocalConfiguration::print(out);
    out << "]";
}

const std::set<std::string> &CmdArgs::keys() const {
    return keys_;
}

const std::vector<std::string> &CmdArgs::args() const {
    return args_;
}

const std::string &CmdArgs::args(size_t i) const {
    ASSERT(i < args_.size());
    return args_[i];
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace option
}  // namespace eckit


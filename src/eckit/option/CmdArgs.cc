/*
 * (C) Copyright 1996- ECMWF.
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


#include "eckit/option/CmdArgs.h"

#include <iostream>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/option/Option.h"
#include "eckit/runtime/Main.h"

namespace eckit::option {

//----------------------------------------------------------------------------------------------------------------------

CmdArgs::CmdArgs(usage_proc usage, int args_count, int minimum_args, bool throw_on_error) {
    init(usage, args_count, minimum_args, throw_on_error);
}

CmdArgs::CmdArgs(usage_proc usage, std::vector<Option*>& options, int args_count, int minimum_args,
                 bool throw_on_error) {
    std::swap(options_, options);  // Take ownership so it can be destroyed
    init(usage, args_count, minimum_args, throw_on_error);
}

CmdArgs::CmdArgs(std::function<void(const std::string&)> usage, std::vector<Option*>& options, int args_count,
                 int minimum_args, bool throw_on_error) {
    std::swap(options_, options);  // Take ownership so it can be destroyed
    init(usage, args_count, minimum_args, throw_on_error);
}

void CmdArgs::init(std::function<void(const std::string&)> usage, int args_count, int minimum_args,
                   bool throw_on_error) {
    Main& ctx  = Main::instance();
    tool_      = ctx.name();
    int argc   = ctx.argc();
    bool error = false;

    std::map<std::string, const option::Option*> opts;

    for (std::vector<option::Option*>::const_iterator j = options_.begin(); j != options_.end(); ++j) {
        if ((*j)->active()) {
            ASSERT(opts.find((*j)->name()) == opts.end());
            opts[(*j)->name()] = *j;
            keys_.insert((*j)->name());
            (*j)->setDefault(*this);
        }
    }

    for (int i = 1; i < argc; ++i) {
        const auto a = ctx.argv(i);
        if (a.size() > 2 && a[0] == '-' && a[1] == '-') {
            auto b = a.begin() + 2;
            auto e = std::find(b, a.end(), '=');

            const std::string key(b, e);

            if (auto j = opts.find(key); j != opts.end()) {
                try {
                    const std::string value(e == a.end() ? "" : std::string{++e, a.end()});
                    value.empty() ? j->second->set(*this) : j->second->set(value, *this);
                }
                catch (UserError&) {
                    Log::info() << "Invalid value for option --" << key << std::endl;
                    error = true;
                }
            }
            else {
                Log::info() << "Invalid option --" << key << std::endl;
                error = true;
            }
        }
        else {
            args_.push_back(a);
        }
    }


    if (args_count >= 0) {
        if (args_.size() != size_t(args_count)) {
            Log::info() << tool_ << ": invalid argument count: expected " << args_count << ", got: " << args_.size()
                        << "." << std::endl;
            error = true;
        }
    }

    if (minimum_args >= 0) {
        if (args_.size() < size_t(minimum_args)) {
            Log::info() << tool_ << ": invalid argument count: expected at least " << minimum_args
                        << ", got: " << args_.size() << std::endl;
            error = true;
        }
    }

    if (error) {
        usage(tool_);
        if (options_.size()) {
            Log::info() << std::endl;
            Log::info() << "Options are:" << std::endl;
            Log::info() << "===========:" << std::endl
                        << std::endl;
            for (std::vector<option::Option*>::const_iterator j = options_.begin(); j != options_.end(); ++j) {
                Log::info() << *(*j) << std::endl
                            << std::endl;
            }
            Log::info() << std::endl;
        }

        if (throw_on_error) {
            for (std::vector<option::Option*>::iterator j = options_.begin(); j != options_.end(); ++j) {
                delete (*j);
            }
            throw UserError("An error occurred in argument parsing", Here());
        }
        ::exit(1);
    }
}


CmdArgs::~CmdArgs() {
    for (std::vector<option::Option*>::iterator j = options_.begin(); j != options_.end(); ++j) {
        delete (*j);
    }
}


void CmdArgs::configure(Configured& c) const {
    for (std::vector<option::Option*>::const_iterator j = options_.begin(); j != options_.end(); ++j) {
        (*j)->copy(*this, c);
    }
}


void CmdArgs::print(std::ostream& out) const {
    out << "CmdArgs[";
    LocalConfiguration::print(out);
    out << "]";
}

// const std::set<std::string>& CmdArgs::keys() const {
//     return keys_;
// }

// const std::vector<std::string>& CmdArgs::args() const {
//     return args_;
// }

const std::string& CmdArgs::operator()(size_t i) const {
    ASSERT(i < args_.size());
    return args_[i];
}

size_t CmdArgs::count() const {
    return args_.size();
}

const std::string& CmdArgs::tool() const {
    return tool_;
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::option

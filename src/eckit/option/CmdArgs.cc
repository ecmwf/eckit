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
#include "eckit/utils/Tokenizer.h"

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
    const Main& ctx = Main::instance();
    tool_           = ctx.name();
    int argc        = ctx.argc();
    bool error      = false;

    using options_map_t = std::map<std::string, const option::Option*>;

    options_map_t opts;

    // Fill in 'keys_' and prepare options map
    for (Option* j : options_) {
        if (j->active()) {
            ASSERT(opts.find(j->name()) == opts.end());
            keys_.insert(j->name());
            opts[j->name()] = j;
            j->setDefault(*this);
        }
    }

    const static std::string prefix = "--";
    // Process all options/values in argv, letting each Option collect the necessary entries
    for (int i = 1; i < argc; ++i) {
        std::string a = ctx.argv(i);
        if (a.substr(0, prefix.size()) == prefix) {  // An Option 'a' is found (starts with '--')!

            // The Option might be formatted as --<name>=<value>
            // ... so we remove the '--' prefix
            a = a.substr(2);
            // ... and tokenize [<name>(,<tail>)]
            std::vector<std::string> tokens = Tokenizer::split_at(a, '=');

            const std::string name = tokens[0];
            tokens.erase(tokens.begin());

            if (auto found = opts.find(name); found != opts.end()) {
                try {
                    const Option* option = found->second;
                    // Given the applicable Option, we prepare the argv tokens (including the <value>)
                    std::vector<std::string> remaining;
                    remaining.reserve(tokens.size() + argc);
                    for (const auto& token : tokens) {
                        remaining.push_back(token);
                    }
                    for (int j = i + 1; j < argc; ++j) {
                        remaining.push_back(ctx.argv(j));
                    }
                    // ... allow the Option to set itself based on all remaining argv tokens
                    size_t consumed = option->set(*this, tokens.size(), std::begin(remaining), std::end(remaining));
                    // ... and, disregard the number of consumed tokens.
                    i += static_cast<int>(consumed - tokens.size());
                }
                catch (UserError&) {
                    Log::info() << "Invalid value for option --" << name << std::endl;
                    error = true;
                }
            }
            else {
                Log::info() << "Invalid option --" << name << std::endl;
                error = true;
            }
        }
        else { // Position argument 'a' is found!
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
            Log::info() << "===========:" << std::endl << std::endl;
            for (const Option* j : options_) {
                Log::info() << *j << std::endl << std::endl;
            }
            Log::info() << std::endl;
        }

        if (throw_on_error) {
            for (const Option* j : options_) {
                delete j;
            }
            throw UserError("An error occurred in argument parsing", Here());
        }
        ::exit(1);
    }
}


CmdArgs::~CmdArgs() {
    for (const Option* j : options_) {
        delete j;
    }
}


void CmdArgs::configure(Configured& c) const {
    for (const Option* j : options_) {
        j->copy(*this, c);
    }
}


void CmdArgs::print(std::ostream& out) const {
    out << "CmdArgs[";
    LocalConfiguration::print(out);
    out << "]";
}

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

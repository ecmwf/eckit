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


#ifndef eckit_option_CmdArgs_H
#define eckit_option_CmdArgs_H

#include <functional>
#include <set>
#include <vector>

#include "eckit/config/LocalConfiguration.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit::option {

//----------------------------------------------------------------------------------------------------------------------

class Option;


class CmdArgs : public LocalConfiguration, private NonCopyable {

public:  // types

    typedef void (*usage_proc)(const std::string& name);

public:  // methods

    CmdArgs(usage_proc usage, int args_count = -1, int minimum_args = 0, bool throw_on_error = false);

    /// Initialise argument parser with a list of options
    /// @note Will take ownership of the contents of the vector, and delete them in destructor
    /// @todo This should probably have some form of smart pointer.
    CmdArgs(usage_proc usage, std::vector<Option*>& options, int args_count = -1, int minimum_args = 0,
            bool throw_on_error = false);

    CmdArgs(std::function<void(const std::string&)> usage, std::vector<Option*>& options, int args_count = -1,
            int minimum_args = 0, bool throw_on_error = false);

    ~CmdArgs();

    // Accessors

    // const std::set<std::string>& keys() const;
    // const std::vector<std::string>& args() const;
    // const std::string& args(size_t) const;

    const std::string& operator()(size_t) const;
    const std::string& tool() const;

    size_t count() const;


    void configure(Configured&) const;

    // has, get and set methods are inherited from LocalConfiguration in their entirety

    std::vector<std::string>::const_iterator begin() const { return args_.begin(); }
    std::vector<std::string>::const_iterator end() const { return args_.end(); }

    std::vector<std::string>::iterator begin() { return args_.begin(); }
    std::vector<std::string>::iterator end() { return args_.end(); }

private:  // methods

    void init(std::function<void(const std::string&)> usage, int args_count, int minumum_args, bool throw_on_errror);

    void print(std::ostream&) const override;

private:  // members

    std::set<std::string> keys_;
    std::vector<std::string> args_;
    std::vector<Option*> options_;

    std::string tool_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::option

#endif  // eckit_option_CmdArgs_H

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


#ifndef eckit_option_CmdArgs_H
#define eckit_option_CmdArgs_H

#include "eckit/config/LocalConfiguration.h"
#include "eckit/memory/NonCopyable.h"

#include <set>
#include <vector>

namespace eckit {
namespace option {

//----------------------------------------------------------------------------------------------------------------------

class Option;


class CmdArgs : public LocalConfiguration, private NonCopyable {

public: // types

    typedef void (*usage_proc)(const std::string& name);

public: // methods

    CmdArgs(usage_proc usage, int args_count = -1, int minimum_args = 0, bool throw_on_error = false);

    /// Initialise argument parser with a list of options
    /// @note Will take ownership of the contents of the vector, and delete them in destructor
    /// @todo This should probably have some form of smart pointer.
    CmdArgs(usage_proc usage, std::vector<Option*>& options, int args_count = -1, int minimum_args = 0, bool throw_on_error = false);

    ~CmdArgs();

    // Accessors

    const std::set<std::string>& keys() const;
    const std::vector<std::string>& args() const;
    const std::string& args(size_t) const;
    const std::string& operator()(size_t) const;
    const std::string& tool() const;

    size_t count() const;

    // has, get and set methods are inherited from LocalConfiguration in their entirety

private: // methods

    void init(usage_proc usage, int args_count, int minumum_args, bool throw_on_errror);

    virtual void print(std::ostream&) const;

private: // members

    std::set<std::string> keys_;
    std::vector<std::string> args_;
    std::vector<Option*> options_;

    std::string tool_;

    bool throwOnError_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace option
}  // namespace eckit

#endif // eckit_option_CmdArgs_H


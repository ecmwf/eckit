/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <string>
#include <vector>

#include "eckit/log/Log.h"
#include "eckit/option/Option.h"
#include "eckit/runtime/Tool.h"


namespace eckit::option {
class CmdArgs;
}


namespace eckit::tools {


class EckitCodecTool : public eckit::Tool {
public:
    using Options = std::vector<option::Option*>;
    using Args    = option::CmdArgs;

protected:
    virtual std::string indent() { return "      "; }
    virtual std::string briefDescription() { return ""; }
    virtual std::string longDescription() { return ""; }
    virtual std::string usage() { return name() + " [OPTION]... [--help,-h]"; }

    void add_option(option::Option* option) { options_.push_back(option); }

    virtual void help(std::ostream& out = Log::info());

    virtual int numberOfPositionalArguments() { return -1; }
    virtual int minimumPositionalArguments() { return numberOfPositionalArguments(); }

    bool handle_help();

public:
    EckitCodecTool(int argc, char** argv);

    int start();

    void run() final {}  // unused

    virtual int execute(const Args&) = 0;

    static constexpr int success() { return 0; }
    static constexpr int failed() { return 1; }

private:
    Options options_;
};


}  // namespace eckit::tools

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/ConfigCmd.h"
#include "eckit/config/Configurable.h"
#include "eckit/config/ResourceMgr.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ConfigCmd::ConfigCmd() :
    CmdResource("config") {}

ConfigCmd::~ConfigCmd() {}

void ConfigCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    if (arg.exists(1) && arg.exists(2)) {
        ResourceMgr::instance().set(arg[1], arg[2]);
        Configurable::reconfigureAll();
    }

    Configurable::dumpAllResources(out);
}

void ConfigCmd::help(std::ostream& out) const {}

Arg ConfigCmd::usage(const std::string& cmd) const {
    return ~(Arg("<resource>", Arg::text) + Arg("<value>", Arg::text));
}

static ConfigCmd configCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

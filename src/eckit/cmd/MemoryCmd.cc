/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/MemoryCmd.h"

#include "eckit/system/MemoryInfo.h"
#include "eckit/system/SystemInfo.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MemoryCmd::MemoryCmd() :
    CmdResource("memory") {}

MemoryCmd::~MemoryCmd() {}

void MemoryCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    out << eckit::system::SystemInfo::instance().memoryUsage() << std::endl;
}

void MemoryCmd::help(std::ostream&) const {}

Arg MemoryCmd::usage(const std::string& cmd) const {
    return Arg();
}

static MemoryCmd memoryCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/MemoryCmd.h"

#include "eckit/system/MemoryInfo.h"
#include "eckit/system/SystemInfo.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MemoryCmd::MemoryCmd() : CmdResource("memory") {}

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

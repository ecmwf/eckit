// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   MemoryCmd.h
/// @author Manuel Fuentes
/// @date   Sep 1998

#ifndef eckit_cmd_MemoryCmd_H
#define eckit_cmd_MemoryCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MemoryCmd : public CmdResource {
public:

    // -- Contructors

    MemoryCmd();

    // -- Destructor

    ~MemoryCmd();

private:

    // No copy allowed

    MemoryCmd(const MemoryCmd&);
    MemoryCmd& operator=(const MemoryCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;

    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

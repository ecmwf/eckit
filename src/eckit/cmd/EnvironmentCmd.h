// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   EnvironmentCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_EnvironmentCmd_H
#define eckit_cmd_EnvironmentCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class EnvironmentCmd : public CmdResource {
public:

    // -- Contructors

    EnvironmentCmd();

    // -- Destructor

    ~EnvironmentCmd();

private:

    // No copy allowed

    EnvironmentCmd(const EnvironmentCmd&);
    EnvironmentCmd& operator=(const EnvironmentCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;
    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

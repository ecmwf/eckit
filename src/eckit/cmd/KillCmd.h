// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   KillCmd.h
/// @author Baudouin Raoult
/// @date   Jan 1997

#ifndef eckit_cmd_KillCmd_H
#define eckit_cmd_KillCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class KillCmd : public CmdResource {
public:

    // -- Contructors

    KillCmd();

    // -- Destructor

    ~KillCmd();

private:

    // No copy allowed

    KillCmd(const KillCmd&);
    KillCmd& operator=(const KillCmd&);

    // -- Methods

    void kill(pid_t, std::ostream&) const;

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;

    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

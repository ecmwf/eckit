// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RemoteCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_RemoteCmd_H
#define eckit_cmd_RemoteCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class RemoteCmd : public CmdResource {
public:

    // -- Contructors

    RemoteCmd();

    // -- Destructor

    ~RemoteCmd();

private:

    // No copy allowed

    RemoteCmd(const RemoteCmd&);
    RemoteCmd& operator=(const RemoteCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;

    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

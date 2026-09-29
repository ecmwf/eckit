// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   ManCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_ManCmd_H
#define eckit_cmd_ManCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ManCmd : public CmdResource {
public:

    // -- Contructors

    ManCmd();

    // -- Destructor

    ~ManCmd();

private:

    // No copy allowed

    ManCmd(const ManCmd&);
    ManCmd& operator=(const ManCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream& out, CmdArg&) override;
    void help(std::ostream&) const override;

    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

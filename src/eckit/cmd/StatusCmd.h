// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   StatusCmd.h
/// @author Baudouin Raoult
/// @date   Jan 1997

#ifndef eckit_cmd_StatusCmd_H
#define eckit_cmd_StatusCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StatusCmd : public CmdResource {
public:

    // -- Contructors

    StatusCmd();

    // -- Destructor

    ~StatusCmd();

private:

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;

    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   StartCmd.h
/// @author Baudouin Raoult
/// @date   Jan 1997

#ifndef eckit_cmd_StartCmd_H
#define eckit_cmd_StartCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StartCmd : public eckit::CmdResource {
public:

    // -- Contructors

    StartCmd();

    // -- Destructor

    ~StartCmd();

private:

    // No copy allowed

    StartCmd(const StartCmd&);
    StartCmd& operator=(const StartCmd&);

    // -- Methods

    void start(std::ostream&, const std::string&) const;

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, eckit::CmdArg&) override;

    void help(std::ostream&) const override;
    eckit::Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

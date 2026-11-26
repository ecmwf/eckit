// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   QuitCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_QuitCmd_H
#define eckit_cmd_QuitCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class QuitCmd : public CmdResource {
public:

    // -- Contructors

    QuitCmd();

    // -- Destructor

    ~QuitCmd();

private:

    // No copy allowed

    QuitCmd(const QuitCmd&);
    QuitCmd& operator=(const QuitCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream& out, CmdArg&) override;
    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   TailCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef TailCmd_H
#define TailCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TailCmd : public CmdResource {
public:

    // -- Contructors

    TailCmd();

    // -- Destructor

    ~TailCmd();

private:

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream& out, CmdArg&) override;
    void help(std::ostream&) const override;

    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

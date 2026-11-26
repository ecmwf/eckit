// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_AliasCmd_H
#define eckit_cmd_AliasCmd_H

#include "eckit/cmd/CmdResource.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class AliasCmd : public CmdResource {
public:

    // -- Contructors

    AliasCmd();

    // -- Destructor

    ~AliasCmd();

private:

    // No copy allowed

    AliasCmd(const AliasCmd&);
    AliasCmd& operator=(const AliasCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;

    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

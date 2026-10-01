// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   HistoryCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_HistoryCmd_H
#define eckit_cmd_HistoryCmd_H

#include "eckit/cmd/CmdResource.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class HistoryCmd : public CmdResource {
public:

    // -- Contructors

    HistoryCmd();

    // -- Destructor

    ~HistoryCmd();

private:

    // No copy allowed

    HistoryCmd(const HistoryCmd&);
    HistoryCmd& operator=(const HistoryCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream& out, CmdArg&) override;
    void help(std::ostream&) const override;

    Arg usage(const std::string& cmd) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

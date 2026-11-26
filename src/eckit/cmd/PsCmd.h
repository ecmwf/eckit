// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_PsCmd_H
#define eckit_cmd_PsCmd_H

#include "eckit/cmd/CmdResource.h"

namespace eckit {

class TaskInfo;

//----------------------------------------------------------------------------------------------------------------------

class PsCmd : public eckit::CmdResource {
public:

    PsCmd();

    ~PsCmd() override;

private:  // methods

    void display(std::ostream&, eckit::TaskInfo&, long, const std::string&) const;
    void display(JSON&, eckit::TaskInfo&, long, const std::string&) const;


    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream& out, eckit::CmdArg&) override;
    void help(std::ostream&) const override;

    eckit::Arg usage(const std::string& cmd) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

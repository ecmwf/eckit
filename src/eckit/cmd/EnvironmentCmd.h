/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   EnvironmentCmd.h
/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_EnvironmentCmd_H
#define eckit_cmd_EnvironmentCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class EnvironmentCmd : public CmdResource {
public:

    // -- Contructors

    EnvironmentCmd();

    // -- Destructor

    ~EnvironmentCmd();

private:

    // No copy allowed

    EnvironmentCmd(const EnvironmentCmd&);
    EnvironmentCmd& operator=(const EnvironmentCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;
    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

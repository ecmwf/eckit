/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

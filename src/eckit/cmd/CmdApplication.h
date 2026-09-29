// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   CmdApplication.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_cmd_CmdApplication_H
#define eckit_cmd_CmdApplication_H

#include "eckit/cmd/CmdParser.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class CmdApplication : public Prompter {
public:

    CmdApplication();
    virtual ~CmdApplication();

protected:

    void execute();

private:

    std::string prompt() const override;
    virtual std::string name() const;

    void startup(std::ostream&);

    void serveMode(long);
    void userMode();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

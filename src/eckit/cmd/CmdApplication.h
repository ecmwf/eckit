/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

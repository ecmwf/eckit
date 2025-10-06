/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

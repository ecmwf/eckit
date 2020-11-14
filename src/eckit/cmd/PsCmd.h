/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

    virtual ~PsCmd() override;

private:  // methods
    void display(std::ostream&, eckit::TaskInfo&, long, const std::string&) const;
    void display(JSON&, eckit::TaskInfo&, long, const std::string&) const;


    // -- Overridden methods

    // From CmdResource

    virtual void execute(std::istream&, std::ostream& out, eckit::CmdArg&) override;
    virtual void help(std::ostream&) const override;

    virtual eckit::Arg usage(const std::string& cmd) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

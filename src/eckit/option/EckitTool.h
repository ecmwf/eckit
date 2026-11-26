// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   Mar 2016

#pragma once

#include "eckit/option/Option.h"
#include "eckit/runtime/Tool.h"

namespace eckit {

namespace option {
class Option;
class CmdArgs;
}  // namespace option

//----------------------------------------------------------------------------------------------------------------------

class EckitTool : public Tool {

protected:  // methods

    EckitTool(int argc, char** argv);

public:

    virtual void usage(const std::string& tool) const = 0;

protected:  // members

    std::vector<eckit::option::Option*> options_;

private:  // methods

    virtual void init(const option::CmdArgs&) {}
    virtual void execute(const option::CmdArgs&) = 0;
    virtual void finish(const option::CmdArgs&) {}

    virtual int numberOfPositionalArguments() const { return -1; }
    virtual int minimumPositionalArguments() const { return -1; }

    void run() override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

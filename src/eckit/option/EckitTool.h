/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

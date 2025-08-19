/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_cmd_LockCmd_H
#define eckit_cmd_LockCmd_H

#include "eckit/cmd/CmdResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class LockCmd : public CmdResource {
public:

    // -- Contructors

    LockCmd();

    // -- Destructor

    ~LockCmd();

private:

    // No copy allowed

    LockCmd(const LockCmd&);
    LockCmd& operator=(const LockCmd&);

    // -- Overridden methods

    // From CmdResource

    void execute(std::istream&, std::ostream&, CmdArg&) override;
    virtual void man(std::ostream&) const;
    void help(std::ostream&) const override;
    Arg usage(const std::string& cmd) const override;
};

inline void destroy(LockCmd**) {}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif  // LockCmd_H

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

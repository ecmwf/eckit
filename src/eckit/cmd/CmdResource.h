// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_cmd_CmdResource_H
#define eckit_cmd_CmdResource_H

#include "eckit/cmd/Arg.h"
#include "eckit/cmd/CmdArg.h"
#include "eckit/eckit.h"


namespace eckit {


class CmdResource {

    using Proc = void (*)(CmdResource*, CmdArg&, std::istream&, std::ostream&);

public:

    // -- Contructors

    CmdResource(const std::string&);

    CmdResource(const CmdResource&)            = delete;
    CmdResource& operator=(const CmdResource&) = delete;
    CmdResource(CmdResource&&)                 = delete;
    CmdResource& operator=(CmdResource&&)      = delete;

    // -- Destructor

    virtual ~CmdResource();

    // -- Class methods

    static bool run(Proc, CmdArg&, std::istream&, std::ostream&);

    // Procs called from CmdParser
    static void command(CmdResource*, CmdArg&, std::istream&, std::ostream&);
    static void loop(CmdResource*, CmdArg&, std::istream&, std::ostream&);
    static void pipe(CmdResource*, CmdArg&, std::istream&, std::ostream&);
    static void redirect(CmdResource*, CmdArg&, std::istream&, std::ostream&);
    static void append(CmdResource*, CmdArg&, std::istream&, std::ostream&);

    static std::vector<std::string> completion(const std::string&);
    static std::vector<std::string> completion(const std::vector<std::string>&);
    static bool completion(const char*, int pos, char*, int);

protected:  // methods

    virtual void execute(std::istream&, std::ostream&, CmdArg&) = 0;

    virtual Arg usage(const std::string& cmd) const = 0;
    virtual void help(std::ostream&) const          = 0;

    virtual void print(std::ostream&) const;

    // -- Class methods

    static void help(std::ostream&, const std::string&);

private:  // members

    using Map = std::map<std::string, CmdResource*, std::less<std::string>>;

    static Map* resources_;

private:  // friends

    friend std::ostream& operator<<(std::ostream& o, const CmdResource& cmd) {
        cmd.print(o);
        return o;
    }
};


}  // namespace eckit

#endif

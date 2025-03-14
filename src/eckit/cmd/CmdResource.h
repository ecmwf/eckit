/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_cmd_CmdResource_H
#define eckit_cmd_CmdResource_H

#include "eckit/cmd/Arg.h"
#include "eckit/cmd/CmdArg.h"
#include "eckit/eckit.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {


class CmdResource : private eckit::NonCopyable {

    typedef void (*Proc)(CmdResource*, CmdArg&, std::istream&, std::ostream&);

public:

    // -- Contructors

    CmdResource(const std::string&);

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

    typedef std::map<std::string, CmdResource*, std::less<std::string> > Map;

    static Map* resources_;

private:  // friends

    friend std::ostream& operator<<(std::ostream& o, const CmdResource& cmd) {
        cmd.print(o);
        return o;
    }
};


}  // namespace eckit

#endif

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   CmdParser.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_cmd_CmdParser_H
#define eckit_cmd_CmdParser_H

#include <iostream>
#include <vector>

#include "eckit/cmd/UserInput.h"

namespace eckit {

class CmdArg;
class CmdResource;
class PathName;
class Value;

//-----------------------------------------------------------------------------

class Prompter {
public:

    virtual std::string prompt() const = 0;
};

class CmdParser {
public:

    // -- Methods

    static void parse(std::istream&, std::ostream&, const Prompter& prompter);
    static void parse(const std::string&, std::ostream&);
    static void parse(const eckit::PathName&, std::ostream&);

    static void unAlias();

    // For commands over shell things
    static void history(const long, std::ostream&);
    static void environment(std::ostream&);
    static void environment(const std::string&, const std::string&);
    static void alias();
    static void alias(const std::string&);
    static void alias(const std::string&, const std::string&);
    static void aliasCompletion(const std::string&, std::vector<std::string>&);
    static void function(const std::string&);
    static void flag(const char, bool);

    static int input();
    static void unput(int);
    static void output(int);

    // On commad line
    static void prompt(const Prompter&);
    static void reset();
    static void addCmd(const char* s);
    static void addCmd(const char c);
    static void historize();

    static void prompt(bool);
    static void console(bool);

    // On startup
    static void flags(const std::string&);

    // Built-in
    static void repeat();
    static void substitute();
    static void shell(const std::string&);

    static void arg(const std::string&, const eckit::Value&);
    static void arg(long, const eckit::Value&);
    static void arg(const eckit::Value&);

    static void var(const std::string&, const eckit::Value&);
    static void var(const std::string&);

    static void run(void (*)(eckit::CmdResource*, eckit::CmdArg&));
    static void run(void (*)(eckit::CmdResource*, eckit::CmdArg&, std::istream&, std::ostream&));

private:

    // No copy allowed

    CmdParser(const CmdParser&);
    CmdParser& operator=(const CmdParser&);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

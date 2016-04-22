/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/ecml/parser/Request.h"
#include "eckit/cmd/UserInput.h"

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"

#include "eckit/ecml/ast/FunctionDefinition.h"
#include "eckit/ecml/ast/Closure.h"
#include "eckit/ecml/prelude/REPLHandler.h"

using namespace std;

namespace eckit {

ExecutionContext* REPLHandler::context_ (0);

REPLHandler::REPLHandler(const string& name)
: SpecialFormHandler(name)
{}

Request REPLHandler::handle(const Request, ExecutionContext& context)
{
    repl(context);

    return new Cell("_list", "", 0, 0);
}


string REPLHandler::historyFile()
{
    return string (getenv("HOME")) + "/.ecml_history";
}

void REPLHandler::readHistory()
{
    eckit::UserInput::loadHistory(historyFile().c_str());
}


void REPLHandler::writeHistory()
{
    eckit::UserInput::saveHistory(historyFile().c_str());
}

bool notInWord(char p)
{
    return p == ' '
        || p == '\t'
        || p == ','
        || p == '('
        || p == ')'
        || p == '/'
        ;
}

void REPLHandler::describe(std::ostream& out, const string& v, Cell* o)
{
    stringstream ss;
    ss << v << ": " << (o ? o->str() : "NULL");

    const string description (ss.str());

    ::write(1, "\r\n", 2);
    ::write(1, description.c_str(), description.size());
    ::write(1, "\r\n", 2);
}

bool REPLHandler::completion(const char* line, int pos, char* insert, int insertmax)
{
    ExecutionContext& context (*REPLHandler::context_); // it would be nice to have it passed as a parameter here

    char *p (const_cast<char*>(line) + pos);
    while (p != line && !notInWord(*(p - 1)))
        --p;
    const string prefix (p);

    ASSERT(REPLHandler::context_);

    const vector<string> matchedVars (context.environment().lookupVariables("^" + prefix));
    const set<string> matched (matchedVars.begin(), matchedVars.end());

    if (matched.empty())
        return true;

    if (matched.size() == 1)
    {
        const string& match (*matched.begin());
        describe(cout, match, context.environment().lookupNoThrow(match)); 

        for (size_t i(prefix.size()), ii(0); ii < insertmax && i < match.size(); ++i)
            insert[ii++] = match[i];

        return true;
    }

    size_t i(0);
    for (set<string>::const_iterator it (matched.begin()); it != matched.end(); ++it)
    {
        const string& ins (*it);
        for (size_t j(0); i < insertmax && j < ins.size(); ++j)
            insert[i++] = ins[j];

        if (i < insertmax)
            insert[i++] = ' ';
    }

    return false;
}

string REPLHandler::readLine(ExecutionContext& context)
{
    // It would be nicer to pass it as a third param to getUserInput
    REPLHandler::context_ = &context; 
    const char* line (eckit::UserInput::getUserInput("ecml> ", eckit::UserInput::completion_proc (&REPLHandler::completion)));
    if (line == 0)
        return "quit";
    return line;
}

void REPLHandler::repl(ExecutionContext& context)
{
    readHistory();
    string cmd; 
    while (true)
    {
        cmd += readLine(context);

        if (cin.eof()
            || cmd == "quit"
            || cmd == "bye")
        {
            writeHistory();
            cout << "Bye." << endl;
            break;
        }

        if (! cmd.size()) continue;

        if (cmd.rfind("\\") == cmd.size() - 1)
        {
            cmd.erase(cmd.size() - 1);
            cmd += '\n';
            continue;
        }

        try {
            Values r (context.execute(cmd));
            cout << " => " << r << endl;
            if (showResultGraph(context))
                r->graph();
            delete r;
        } catch (Exception e) {
            // error message is already printed by Exception ctr
            //cout << "*** error: " << e.what() << endl;
        }
        cmd = "";
    }
}

bool REPLHandler::showResultGraph(ExecutionContext& context)
{
    if (! context.environment().lookupNoThrow("show_dot"))
        return false;
    else
    {
        vector<string> vs (context.environment().lookupList("show_dot", context));
        return vs.size() == 1 && vs[0] == "true";
    }
}

} // namespace eckit

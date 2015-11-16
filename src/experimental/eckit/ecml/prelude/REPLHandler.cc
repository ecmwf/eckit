/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/ecml/parser/Request.h"

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"

#include "eckit/ecml/ast/FunctionDefinition.h"
#include "eckit/ecml/ast/Closure.h"
#include "eckit/ecml/prelude/REPLHandler.h"

using namespace std;

namespace eckit {

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
#ifdef ECKIT_HAVE_READLINE
    int rc (read_history(historyFile().c_str()));
    if (rc)
        Log::warning() << "read_history => " << rc << endl;
#endif
}


void REPLHandler::writeHistory()
{
#ifdef ECKIT_HAVE_READLINE
    int rc (write_history(historyFile().c_str()));
    if (rc)
        Log::warning() << "write_history => " << rc << endl;
#endif
}

string REPLHandler::readLine()
{
#ifdef ECKIT_HAVE_READLINE
    char* p (readline("ecml> "));
    if (! p)
        return "bye";
    add_history(p);
    string r (p);
    free(p);
    return r;
#else
    string r;
    cout << "ecml> ";
    getline (cin, r);
    return r;
#endif
}

void REPLHandler::repl(ExecutionContext& context)
{
    readHistory();
    string cmd; 
    while (true)
    {
        cmd += readLine();

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

/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/eckit_config.h"

// There is also ECKIT_HAVE_READLINE, not sure which one is better to use 
#ifdef ECKIT_HAVE_READLINE
# include <readline/readline.h>
# include <readline/history.h>
#endif

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "eckit/runtime/Context.h"

using namespace std;
using namespace eckit;

class TestECML : public Tool {
public:
    TestECML (int argc, char **argv) : Tool(argc, argv) {}
    ~TestECML() {}
    
    virtual void run();

    void repl(ExecutionContext& context);

    std::string readLine();
    void writeHistory();
    void readHistory();
    string historyFile();

protected:
    virtual void runScript(const std::string& pathName);
    bool showResultGraph(ExecutionContext& context) const;
};

void TestECML::run()
{
    int argc (Context::instance().argc());
    if (argc < 2)
    {
        //throw UserError("Command line required (name(s) of file(s) with ECML script");
        ExecutionContext context;
        repl(context);
    }

    for (size_t i(1); i < Context::instance().argc(); ++i)
        runScript(Context::instance().argv(i));
}

void TestECML::runScript(const string& pathName)
{
    Log::info() << endl << " ** ecml_test: running " << pathName << endl;
    ExecutionContext context;
    context.executeScriptFile(pathName);
}

string TestECML::historyFile()
{
    return string (getenv("HOME")) + "/.ecml_history";
}

void TestECML::readHistory()
{
#ifdef ECKIT_HAVE_READLINE
    int rc (read_history(historyFile().c_str()));
    if (rc)
        Log::warning() << "read_history => " << rc << endl;
#endif
}


void TestECML::writeHistory()
{
#ifdef ECKIT_HAVE_READLINE
    int rc (write_history(historyFile().c_str()));
    if (rc)
        Log::warning() << "write_history => " << rc << endl;
#endif
}

string TestECML::readLine()
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

bool TestECML::showResultGraph(ExecutionContext& context) const
{
    if (! context.environment().lookupNoThrow("show_dot"))
        return false;
    else
    {
        vector<string> vs (context.environment().lookupList("show_dot", context));
        return vs.size() == 1 && vs[0] == "true";
    }
}

void TestECML::repl(ExecutionContext& context)
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

int main(int argc,char **argv)
{
    //TODO: set a behaviour?
    //Context::instance().behavior( new odb::ODBBehavior() );
    // TODO: enable $DEBUG (Log::debug)

    TestECML runner(argc, argv);
    runner.start();
    return 0;
}

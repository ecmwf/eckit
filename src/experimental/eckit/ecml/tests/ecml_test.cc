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

protected:
    virtual void runScript(const std::string& pathName);
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

string TestECML::readLine()
{
#ifdef ECKIT_HAVE_READLINE
    char* p (readline("ecml> "));
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

void TestECML::repl(ExecutionContext& context)
{
    while (true)
    {
        string cmd (readLine());

        if (cin.eof() // Not sure this is working when readline enabled 
            || cmd == "quit"
            || cmd == "bye")
        {
            cout << endl << "Bye." << endl;
            break;
        }

        try {
            Values r (context.execute(cmd));
            cout << " => " << r << endl;
        } catch (Exception e) {
            // error message is already printed by Exception ctr
            //cout << "*** error: " << e.what() << endl;
        }
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

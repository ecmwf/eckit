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

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/runtime/Context.h"

#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Module.h"
#include "experimental/eckit/ecml/prelude/PrintHandler.h"

using namespace std;
using namespace eckit;

class ECMLUnitTests : public Tool {
public:
    ECMLUnitTests (int argc, char **argv) : Tool(argc, argv) {}
    ~ECMLUnitTests() {}
    
    virtual void run();
protected:
    virtual void runTests();
};

class TestPrintHandler : public PrintHandler
{ 
public:
    TestPrintHandler(ostream& o, const string& name, const string& end)
    : PrintHandler(name, end)
    { out(o); }
};

struct ECMLTestModule : public Module {
    ECMLTestModule()
    : Module(),
      s_(),
      println_(s_, "tprintln", "\n"),
      print_(s_, "tprint", "")
    {}

    void importInto(ExecutionContext& context)
    {
        context.registerHandler("print", print_);
        context.registerHandler("println", println_);
    }

    string str() { return s_.str(); }

    void clear()
    {
        s_.str("");
        s_.clear();
    }

private:
    stringstream s_;
    TestPrintHandler println_;
    TestPrintHandler print_;
};

void ECMLUnitTests::run() { runTests(); }

void ECMLUnitTests::runTests()
{
    Log::info() << endl << " ** ecml_unittests **" << endl;

    ECMLTestModule tm;
    ExecutionContext context;
    tm.importInto(context);

    Cell* c (context.copy());
    //stringstream ss;
    //ss << c << endl;
    //cout << ss.str();

    context.execute("print, values = Hello world");
    ASSERT(tm.str() == "Hello world ");
    tm.clear();
    context.execute("print, values = Hello again");
    ASSERT(tm.str() == "Hello again ");
}

int main(int argc,char **argv)
{
    //TODO: set a behaviour?
    //Context::instance().behavior( new odb::ODBBehavior() );
    // TODO: enable $DEBUG (Log::debug)

    ECMLUnitTests runner(argc, argv);
    runner.start();
    return 0;
}

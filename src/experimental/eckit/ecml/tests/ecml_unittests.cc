/*
 * (C) Copyright 1996-2015 ECMWF.
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
#include "eckit/io/PartFileHandle.h"

#include "eckit/runtime/Context.h"

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Interpreter.h"
#include "eckit/ecml/core/Module.h"
#include "eckit/ecml/prelude/PrintHandler.h"
#include "eckit/ecml/prelude/DefineFunctionHandler.h"
#include "eckit/ecml/parser/RequestParser.h"
#include "eckit/ecml/ast/FunctionDefinition.h"
#include "eckit/ecml/ast/Closure.h"
#include "eckit/ecml/data/DataHandleFactory.h"
#include "eckit/ecml/data/PartFileHandleFactory.h"

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
    ostream& L (Log::info());

    L << endl << " ** ecml_unittests **" << endl;

    ECMLTestModule tm;
    ExecutionContext context;
    tm.importInto(context);

    //Cell* c (context.copy());
    //stringstream ss;
    //ss << c << endl;
    //cout << ss.str();

    context.execute("print, values = Hello world");
    ASSERT(tm.str() == "Hello world");
    tm.clear();
    context.execute("print, values = Hello again");
    ASSERT(tm.str() == "Hello again"); // not sure where the trailing space come from


    // Closure
    Cell* requests (RequestParser::parse("function,of=x,capture=a/b,f=(println,values=FOO)"));
    Cell* parsedFunction (requests->value());
    //L << "parsedFunction: " << parsedFunction << endl;

    FunctionDefinition funDef(parsedFunction);
    //L << "funDef.parameters().size(): " << funDef.parameters().size() << endl;

    ASSERT(funDef.parameters().size() == 1);
    ASSERT(funDef.parameters()[0] == "x");
    ASSERT(funDef.capturedVariables().size() == 2);
    ASSERT(funDef.capturedVariables()[0] == "a");
    ASSERT(funDef.capturedVariables()[1] == "b");
    ASSERT(funDef.code()->str() == "(println, values = \"FOO\")");

    Cell* rs (RequestParser::parse("closure,name=f,parameters=x,captured=(environment,a=1,b=2),code=(println,values=FOO)"));
    //L << "rs: " << rs << endl;
    Cell* cc (rs->value());
    //L << "cc: " << cc << endl;
    Closure clos (cc);
    //L << "clos:" << (Cell*) clos << endl;

    DefineFunctionHandler df("function");
    context.execute("let, a=1,b=2");
    Cell* cclosure (df.handle(parsedFunction, context));
    cclosure = cclosure->value();
    //L << "cclosure: " << cclosure << endl;
    Closure closure (cclosure);
    //L << "closure: " << (Cell*) closure << endl;
    //((Cell*) closure)->graph();
    //ASSERT(cclosure->str() == ((Cell*) closure)->str());
    context.execute("function,of=name,capture=a/b,greetings=(print,values=Hello /(value,of=name)/(value,of=a)/(value,of=b))");
    context.execute("greetings, name=Piotr");
    context.execute(
    "function, foo = ("
    "    function, bar = ( println, values = BAR )"
    "    value, of = bar"
    ")"
    );
    //c = context.interpreter().eval(RequestParser::parse("foo"), context);
    //c = context.interpreter().eval(RequestParser::parse("value, of = bar"), context);
    Cell* c;
    Cell* parsed ( RequestParser::parse("let, f = ( foo ) ") );

    //parsed->graph("parsed 'let, f = ( foo )': " + parsed->str());

    context.interpreter().debug(true);
    c = context.interpreter().eval(parsed, context);

    //c->graph("evaluated: 'let, f = ( foo )' " + c->str());

    c = context.interpreter().eval(RequestParser::parse("f"), context);
    //c->graph("evaluated 'f': " + c->str());


    context.execute("system, values = 'echo some test data for your testing pleasure > test_data_for_ecml_part_file_testing.txt'");
    string descriptor ("partfile://test_data_for_ecml_part_file_testing.txt:0,10");
    DataHandle* dh (DataHandleFactory::openForRead(descriptor));
    Log::info () << "partfile: " << dh << endl;
    PartFileHandle* pfh (dynamic_cast<PartFileHandle*>(dh));
    ASSERT(pfh);
    context.execute("system, values = 'rm -f test_data_for_ecml_part_file_testing.txt'");
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

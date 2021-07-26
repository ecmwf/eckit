/*

BUG ECKIT-175
=============

    when Main::initialise(argc,argv) is *not*
    called or a Tool is *not* used,
    then everything is OK, and the
    FileTarget gets destructed as opposed to
    when Main::initialise(argc,argv) *is* called,
    or a Tool *is* used.

    Program expects an integer argument. Possible values:
    0 :
        Test does *not* initialise a Main::instance()
        Everything works as expected.

    1 :
        Test explicitely starts with Main::initialise(argc,argv)
        Buffers should be flushed but aren't
    2 :
        Test uses a eckit::Tool to initialise a Main::instance()
        Buffers should be flushed but aren't

Note:
    The destructor ~FileTarget() is modified to print
    a message when invoked, so don't merge this

*/

#include <cstdlib>
#include <iostream>
#include <map>

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Tool.h"

using namespace eckit;

void myrun() {

    Log::setFile("ECKIT-175.out");

    Log::info() << "Flushed message in myrun()" << std::endl;
    Log::info() << "NON Flushed message in myrun()\n";
}

class MyTool : public Tool {
public:
    MyTool(int argc, char** argv) :
        Tool(argc, argv) {}
    virtual ~MyTool() {}
    virtual void run() { myrun(); }
};


enum
{
    NO_INIT = 1,
    INIT    = 2,
    TOOL    = 3
};


int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <case> " << std::endl;
        std::cout << "       Case values: 1 2 3" << std::endl;
        return 1;
    }

    int test = std::atoi(argv[1]);

    switch (test) {

        case 1: {
            myrun();
            break;
        }

        case 2: {
            Main::initialise(argc, argv);
            myrun();
            break;
        }

        case 3: {
            MyTool tool(argc, argv);
            tool.start();
            break;
        }

        default:
            exit(1);
            break;
    }

    return 0;
}

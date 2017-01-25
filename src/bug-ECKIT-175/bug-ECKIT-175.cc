
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

#include <map>
#include <cstdlib>
#include <iostream>
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Tool.h"
#include "eckit/log/Log.h"

using namespace eckit;

void myrun() {
  Log::setFile("bug-ECKIT-175.out");

  Log::info() << "Flushed message due to std::endl.\n"
              << "More should follow...\n"
              << std::endl;

/*
            IF FOLLOWING MESSAGE DOES NOT GET PRINTED
            IN FILE "bug-ECKIT-175.out" THERE IS A PROBLEM
*/
  Log::info() << "Unflushed message should appear regardless\n"
              << "in file when channel gets destroyed.\n\n"
              << "EOF";
}

class MyTool : public Tool {
public:
  MyTool(int argc, char **argv) : Tool(argc,argv) {}
  virtual ~MyTool()  { Log::info() << "~MyTool()\n"; }
  virtual void run() { myrun(); }
};

enum { NO_INIT=0, INIT=1, TOOL=2 };
std::string test_description(int t) {
  if( t == NO_INIT ) {
    return "    Test does *not* initialise a Main::instance()\n"
           "    Everything works as expected.\n";
  }
  if( t == INIT ) {
    return "    Test explicitely starts with Main::initialise(argc,argv)\n"
           "    Buffers should be flushed but aren't";
  }
  if( t == TOOL ) {
    return "    Test uses a eckit::Tool to initialise a Main::instance()\n"
           "    Buffers should be flushed but aren't";
  }
}

int main(int argc, char **argv) {

  if( argc == 1 ) {
    std::cout << "Program expects an integer argument. Possible values: \n";
    std::cout << NO_INIT << " :\n" << test_description(NO_INIT) << std::endl;
    std::cout << INIT    << " :\n" << test_description(INIT)    << std::endl;
    std::cout << TOOL    << " :\n" << test_description(TOOL)    << std::endl;
    return 1;
  }
  int test = std::atoi(argv[1]);
  std::cout << test_description(test) << std::endl;
  std::cout << "    Please check content of 'bug-ECKIT-175.out'\n";

  switch( test ) {
    case NO_INIT: {
      myrun();
      break;
    }
    case INIT: {
      Main::initialise(argc,argv);
      myrun();
      break;
    }
    case TOOL: {
      MyTool tool(argc,argv);
      tool.start();
      break;
    }
    default:
      break;
  }

  return 0;
}

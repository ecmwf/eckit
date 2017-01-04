
/*

BUG ECKIT-175
=============

     when Main::initialise(argc,argv) is *not*
     called, then everything is OK, and the 
     FileTarget gets destructed as opposed to
     when Main::initialise(argc,argv) *is* called.

     The destructor ~FileTarget() is modified to print
     a message when invoked, so don't merge this

*/

#include "eckit/runtime/Main.h"
#include "eckit/log/Log.h"

using namespace eckit;

int main(int argc, char **argv) {

// Main::initialise(argc,argv);

  Log::setFile("bug-ECKIT-175.out");

  Log::info() << "Flushed message due to std::endl.\n" << std::endl;

  Log::info() << "Unflushed message should appear regardless\n"
              << "in file when channel gets destroyed.";

  return 0;
}

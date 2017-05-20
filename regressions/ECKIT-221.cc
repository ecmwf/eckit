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

#include "eckit/memory/ScopedPtr.h"
#include "eckit/io/DataHandle.h"
#include "eckit/mpi/Comm.h"
#include "eckit/utils/Translator.h"
#include "eckit/runtime/Tool.h"

using namespace eckit;

class MyTool : public Tool {
public:

    MyTool(int argc, char **argv) : Tool(argc,argv) {}

    virtual ~MyTool() {
        eckit::mpi::finalizeAllComms();
    }

    virtual void run() {

        PathName filename(argv()[1]);

        eckit::SharedBuffer buffer = eckit::mpi::comm().broadcastFile(filename, 0);

        PathName out = filename + "." + Translator<size_t, std::string>()(eckit::mpi::comm().rank());

        ScopedPtr<DataHandle> dh ( out.fileHandle() ); AutoClose closer(*dh);
        dh->openForWrite(buffer.size());
        dh->write(buffer, buffer.size());
    }

};

int main(int argc, char **argv) {

    if( argc != 2 ) {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return -1;
    }

    MyTool tool(argc,argv);
    return tool.start();
}

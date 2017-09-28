/*

FEATURE ECKIT-221
=================

    Test broadcasting of file, read on one processor

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
        mpi::finaliseAllComms();
    }

    virtual void run() {

        PathName filename(argv()[1]);

        SharedBuffer buffer = mpi::comm().broadcastFile(filename, 0);

        PathName out = filename + "." + Translator<size_t, std::string>()(mpi::comm().rank());

        ScopedPtr<DataHandle> dh ( out.fileHandle() ); AutoClose closer(*dh);
        dh->openForWrite(buffer.size());
        dh->write(buffer.data(), buffer.size());
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

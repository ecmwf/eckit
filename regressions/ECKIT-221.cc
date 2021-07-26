/*

FEATURE ECKIT-221
=================

    Test broadcasting of file, read on one processor

*/

#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/mpi/Comm.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Translator.h"

using namespace eckit;

class MyTool : public Tool {
public:
    MyTool(int argc, char** argv) :
        Tool(argc, argv) {}

    virtual ~MyTool() { mpi::finaliseAllComms(); }

    virtual void run() {

        PathName filename(argv()[1]);

        SharedBuffer buffer = mpi::comm().broadcastFile(filename, 0);

        PathName out = filename + "." + Translator<size_t, std::string>()(mpi::comm().rank());

        std::unique_ptr<DataHandle> dh(out.fileHandle());
        AutoClose closer(*dh);
        dh->openForWrite(buffer.size());
        dh->write(buffer.data(), buffer.size());
    }
};

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return -1;
    }

    MyTool tool(argc, argv);
    return tool.start();
}

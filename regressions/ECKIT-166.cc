/*

BUG ECKIT-166
=============

*/

#include <iostream>

#include "eckit/mpi/Comm.h"
#include "eckit/runtime/Main.h"

using namespace eckit;

void run(int argc, char** argv) {

    Main::initialise(argc, argv);

    // This should initialise MPI automatically
    if (mpi::comm().rank() == 0)
        std::cout << "mpi::comm().size() = " << mpi::comm().size() << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "begin of main" << std::endl;
    run(argc, argv);
    std::cout << "end of main" << std::endl;
    return 0;
}

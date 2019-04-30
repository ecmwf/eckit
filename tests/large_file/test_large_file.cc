/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <limits>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Bytes.h"
#include "eckit/testing/Test.h"

#define TOTAL_SIZE (SIZE_2G + SIZE_1M)  // 2Gb + 1Mb file
// #define TOTAL_SIZE  2*SIZE_2G // 4Gb file
#define SIZE_2G 2147483648  // 2^31
#define SIZE_1M 1048576     // 2^20

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_large_file") {
    std::cout << "off_t can index " << Bytes(std::pow(2.0, (int)sizeof(off_t) * 8)) << std::endl;

    // make temporary file name
    int fd;
    char filename[] = "large.XXXXXX";
    if ((fd = ::mkstemp(filename)) == -1)
        ::perror("cannot create temporary file"), ::exit(EXIT_FAILURE);

    std::cout << "openning file '" << filename << "'" << std::endl;

    // file is opened in mkstemp, so this is not needed
    //    if( ( fd = ::open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600) ) == -1 )
    //      perror("Error opening file"),  exit(EXIT_FAILURE);

    // stretch the file size
    if ((long long)::lseek(fd, TOTAL_SIZE - 1, SEEK_SET) < 0)
        ::perror("Error calling lseek() to 'stretch' the file"), ::exit(EXIT_FAILURE);

    // write something to the end of the file to actually resize it correctly
    if (::write(fd, "", 1) != 1)
        ::close(fd), perror("Error writing last byte of the file"), ::exit(EXIT_FAILURE);

    // lock the region beyond the 2GB limit until the EOF

    struct flock fl;

    fl.l_type   = F_WRLCK;   // F_RDLCK, F_WRLCK, F_UNLCK
    fl.l_whence = SEEK_SET;  // SEEK_SET, SEEK_CUR, SEEK_END
    fl.l_start  = SIZE_2G;   // Offset from l_whence
    fl.l_len    = 0;         // length, 0 = to EOF
    fl.l_pid    = getpid();  // PID

    fcntl(fd, F_SETLKW, &fl);

    // unlock the region
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);

    eckit::PathName file(filename);

    file.unlink();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}

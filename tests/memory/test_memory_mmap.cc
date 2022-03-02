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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"
#include "eckit/memory/MMap.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

int get_pagesize() {
    int pagesize;
    errno = 0ll;
    if ((pagesize = sysconf(_SC_PAGE_SIZE)) == -1) {
        if (errno == 0)
            printf("PAGE_SIZE not supported by sysconf implementation\n");
        else
            perror("sysconf error.");

        exit(EXIT_FAILURE);
    }
    return pagesize;
}

// |--------------------------------|--------------------------------|
//                                  ^2G limit                        ^4Gb
//                              ^--------^ mapped region

#define SIZE_2G 2147483648      // 2^31
#define TOTAL_SIZE 2 * SIZE_2G  // 4Gb file

#define ELEMS_2G (SIZE_2G / sizeof(int))

#define NELEMS 8 * 1024
#define MAP_SIZE (NELEMS * sizeof(int))

#define ELEM_LW (ELEMS_2G - NELEMS / 2)
#define ELEM_UP (ELEM_LW + NELEMS)

#define SIZE_UP (ELEM_UP * sizeof(int))
#define SIZE_LW (ELEM_LW * sizeof(int))

//----------------------------------------------------------------------------------------------------------------------

CASE("Test memory map") {
    // compute the 2GB limit and its number of pages
    int pagesize       = get_pagesize();
    size_t nbpages_2gb = SIZE_2G / pagesize;
    EXPECT(nbpages_2gb * pagesize == SIZE_2G);

    std::cout << Bytes(SIZE_2G) << " are " << nbpages_2gb << " memory pages" << std::endl;

    // mmapped array of int's
    int* map;

    // make temporary file name
    int fd;
    char filename[] = "mmaped.XXXXXX";
    if ((fd = ::mkstemp(filename)) == -1)
        perror("cannot create temporary file"), exit(EXIT_FAILURE);

    // file is opened in mkstemp, so this is not needed
    //    if( ( fd = ::open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600) ) == -1 )
    //      perror("Error opening file"),  exit(EXIT_FAILURE);

    // stretch the file size to the size of the (mmapped) array of ints
    if ((long long)::lseek(fd, TOTAL_SIZE - 1, SEEK_SET) < 0)
        perror("Error calling lseek() to 'stretch' the file"), exit(EXIT_FAILURE);

    // write something to the end of the file to actually resize it correctly
    if (::write(fd, "", 1) != 1)
        close(fd), perror("Error writing last byte of the file"), exit(EXIT_FAILURE);

    // map the file
    std::cout << "mapping " << Bytes(MAP_SIZE) << " from " << Bytes(SIZE_LW) << " to " << Bytes(SIZE_UP) << std::endl;

    if ((map = (int*)MMap::mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SIZE_LW)) == MAP_FAILED)
        close(fd), perror("Error mmapping the file"), exit(EXIT_FAILURE);

    // write to file as if it were memory
    for (int i = 0; i < NELEMS; ++i)
        map[i] = 2 * i;

    // free the mmapped memory
    if (MMap::munmap(map, MAP_SIZE) == -1)
        perror("Error un-mmapping the file"), exit(EXIT_FAILURE);

    // close the file
    if (close(fd) == -1)
        perror("Error closing file descriptot"), exit(EXIT_FAILURE);

    // test that the contents of the file are correct

    // remove the file -- its large!
    if (::unlink(filename) == -1)
        perror("Error removing the file"), exit(EXIT_FAILURE);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileLock.h"
#include "eckit/thread/AutoLock.h"

#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_io_filelock") {
    const PathName path("test.lock");
    if (path.exists()) {
        path.unlink();
    }

    {
        ASSERT(!path.exists());

        {
            FileLock file(path);
            AutoLock<FileLock> lock(file);

            EXPECT(path.exists());
        }

        EXPECT(path.exists());

        path.unlink(true);
    }

    {
        ASSERT(!path.exists());

        {
            FileLock file(path, true);
            AutoLock<FileLock> lock(file);

            EXPECT(path.exists());
        }

        EXPECT(!path.exists());
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}

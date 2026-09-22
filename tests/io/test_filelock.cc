// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileLock.h"
#include "eckit/testing/Test.h"
#include "eckit/thread/AutoLock.h"

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

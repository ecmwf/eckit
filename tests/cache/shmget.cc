// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/ipc.h>

#include <cerrno>
#include <fstream>

#include "eckit/cache/Shmget.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


PathName unique_file() {
    auto path = PathName::unique(PathName("shmget"));
    std::ofstream(path.localPath()) << "content";
    return path;
}


CASE("Shmget: key of a file") {
    const auto path = unique_file();

    EXPECT(calculate_shmget_key_from_path(path) == calculate_shmget_key_from_path(path));
    EXPECT_THROWS(calculate_shmget_key_from_path(PathName(path.asString() + ".missing")));

    path.unlink(false);
}


CASE("Shmget: segment created, attached, accounted for, detached and removed") {
    constexpr size_t size = 4096;

    const auto path = unique_file();
    const auto key  = calculate_shmget_key_from_path(path);

    const auto shmid = Shmget::create(key, size);
    if (shmid < 0) {
        Log::warning() << "shared memory not available, skipping" << std::endl;
        path.unlink(false);
        return;
    }

    // the same segment, by key, but not created again
    EXPECT(Shmget::shmget(key, 0, 0) == shmid);
    EXPECT(Shmget::create(key, size) < 0 && errno == EEXIST);

    const auto before = Shmget::info();

    auto* address = Shmget::shmat(shmid, nullptr, 0);
    EXPECT(address != reinterpret_cast<void*>(-1));

    const auto attached = Shmget::info();
    EXPECT(attached.count == before.count + 1);
    EXPECT(attached.size == before.size + size);

    EXPECT(Shmget::shmdt(address) == 0);

    const auto after = Shmget::info();
    EXPECT(after.count == before.count);
    EXPECT(after.size == before.size);

    EXPECT(Shmget::shmctl(shmid, IPC_RMID, nullptr) == 0);
    EXPECT(Shmget::shmget(key, 0, 0) < 0);

    path.unlink(false);
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}

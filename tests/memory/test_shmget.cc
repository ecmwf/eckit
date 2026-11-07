// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <sys/ipc.h>

#include <cerrno>
#include <cstddef>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/memory/Shmget.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

constexpr size_t SIZE = 4096;

const auto* const NOT_ATTACHED = reinterpret_cast<void*>(-1);

// A new segment, removed at the end of the test
class Segment {
public:

    explicit Segment(int mode = 0600) : id(Shmget::shmget(IPC_PRIVATE, SIZE, IPC_CREAT | mode)) {}

    Segment(const Segment&)            = delete;
    Segment& operator=(const Segment&) = delete;

    ~Segment() {
        if (id >= 0) {
            Shmget::shmctl(id, IPC_RMID, nullptr);
        }
    }

    bool available() const {
        if (id < 0) {
            Log::warning() << "shared memory not available, skipping" << std::endl;
        }
        return id >= 0;
    }

    const int id;
};

struct Accounted {
    Accounted() { Shmget::info(count, size); }

    size_t count = 0;
    size_t size  = 0;
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("Shmget: segment created with the given permissions") {
    for (const int mode : {0600, 0644}) {
        Segment segment(mode);
        if (!segment.available()) {
            return;
        }

        shmid_ds buf{};
        EXPECT(Shmget::shmctl(segment.id, IPC_STAT, &buf) == 0);
        EXPECT(static_cast<int>(buf.shm_perm.mode & 0777) == mode);
        EXPECT(static_cast<size_t>(buf.shm_segsz) == SIZE);
    }
}

CASE("Shmget: segment accounted for until detached") {
    Segment segment;
    if (!segment.available()) {
        return;
    }

    const Accounted before;

    auto* address = Shmget::shmat(segment.id, nullptr, 0);
    EXPECT(address != NOT_ATTACHED);

    const Accounted attached;
    EXPECT(attached.count == before.count + 1);
    EXPECT(attached.size == before.size + SIZE);

    EXPECT(Shmget::shmdt(address) == 0);

    const Accounted after;
    EXPECT(after.count == before.count);
    EXPECT(after.size == before.size);
}

CASE("Shmget: an existing segment is accounted for by its size, when got without it") {
    // of a unique file, not IPC_PRIVATE, so it can be got by key
    const auto path = PathName::unique(PathName("shmget"));
    path.touch();
    const auto key = ::ftok(path.localPath(), 1);
    EXPECT(key != static_cast<key_t>(-1));

    const int id = Shmget::shmget(key, SIZE, IPC_CREAT | IPC_EXCL | 0600);
    if (id < 0) {
        Log::warning() << "shared memory not available, skipping" << std::endl;
        path.unlink(false);
        return;
    }

    // not created again
    EXPECT(Shmget::shmget(key, SIZE, IPC_CREAT | IPC_EXCL | 0600) < 0 && errno == EEXIST);

    const auto got = Shmget::shmget(key, 0, 0);
    EXPECT(got == id);

    const Accounted before;

    auto* address = Shmget::shmat(got, nullptr, 0);
    EXPECT(address != NOT_ATTACHED);

    const Accounted attached;
    EXPECT(attached.size == before.size + SIZE);

    EXPECT(Shmget::shmdt(address) == 0);
    EXPECT(Shmget::shmctl(id, IPC_RMID, nullptr) == 0);
    EXPECT(Shmget::shmget(key, 0, 0) < 0);

    path.unlink(false);
}

CASE("Shmget: a segment attached twice is accounted for twice") {
    Segment segment;
    if (!segment.available()) {
        return;
    }

    const Accounted before;

    auto* first  = Shmget::shmat(segment.id, nullptr, 0);
    auto* second = Shmget::shmat(segment.id, nullptr, 0);
    EXPECT(first != NOT_ATTACHED);
    EXPECT(second != NOT_ATTACHED);

    const Accounted attached;
    EXPECT(attached.count == before.count + 2);
    EXPECT(attached.size == before.size + 2 * SIZE);

    EXPECT(Shmget::shmdt(first) == 0);

    const Accounted one;
    EXPECT(one.count == before.count + 1);
    EXPECT(one.size == before.size + SIZE);

    EXPECT(Shmget::shmdt(second) == 0);

    const Accounted after;
    EXPECT(after.count == before.count);
    EXPECT(after.size == before.size);
}

CASE("Shmget: failures are not accounted for") {
    const Accounted before;

    EXPECT(Shmget::shmat(-1, nullptr, 0) == NOT_ATTACHED);

    const char not_attached = 0;
    EXPECT(Shmget::shmdt(&not_attached) != 0);

    const Accounted after;
    EXPECT(after.count == before.count);
    EXPECT(after.size == before.size);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}

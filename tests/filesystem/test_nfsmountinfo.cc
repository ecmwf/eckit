// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <initializer_list>
#include <optional>
#include <sstream>
#include <string>

#include "eckit/filesystem/NFSMountInfo.h"
#include "eckit/testing/Test.h"

using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

// Lines as the kernel prints them (fs/nfs/super.c, nfs_show_mount_options) in /proc/self/mountinfo.
constexpr const char* nfs4Shared =
    "583 515 0:60 / /mnt/nfs4 rw,relatime master:175 - nfs4 :/ rw,vers=4.0,rsize=1048576,wsize=1048576,namlen=255,"
    "hard,proto=tcp,timeo=600,retrans=2,sec=sys,clientaddr=127.0.0.1,local_lock=none,addr=127.0.0.1";
constexpr const char* nfs3Shared =
    "590 515 0:61 / /mnt/shared rw,relatime - nfs 10.0.0.1:/x rw,vers=3,hard,proto=tcp,sec=sys,local_lock=none,"
    "addr=10.0.0.1";
constexpr const char* nfs3NoLock =
    "591 515 0:62 / /mnt/nolock rw,relatime shared:200 - nfs 10.0.0.1:/x rw,vers=3,hard,nolock,proto=tcp,sec=sys,"
    "mountvers=3,mountproto=tcp,local_lock=all,addr=10.0.0.1";
constexpr const char* nfs3Posix =
    "592 515 0:63 / /mnt/posix rw - nfs 10.0.0.1:/x rw,vers=3,hard,proto=tcp,sec=sys,local_lock=posix,addr=10.0.0.1";
constexpr const char* nfs3Flock =
    "593 515 0:64 / /mnt/flock rw - nfs 10.0.0.1:/x rw,vers=3,hard,proto=tcp,sec=sys,local_lock=flock,addr=10.0.0.1";
// Kernels that do not print local_lock= still print nolock.
constexpr const char* nfs3Legacy =
    "594 515 0:65 / /mnt/legacy rw - nfs 10.0.0.1:/x rw,vers=3,hard,nolock,proto=tcp,sec=sys,addr=10.0.0.1";
// The kernel lets nolock override local_lock= (nfs_try_mount_request) and prints local_lock=all for it. If a line still
// lists nolock next to another local_lock= value, nolock decides, in whatever order they appear.
constexpr const char* nfs3NoLockAndLocalLockNone =
    "597 515 0:68 / /mnt/nolock-none rw - nfs 10.0.0.1:/x rw,vers=3,hard,nolock,proto=tcp,sec=sys,local_lock=none,"
    "addr=10.0.0.1";
constexpr const char* nfs3NoLockAndLocalLockFlock =
    "598 515 0:69 / /mnt/nolock-flock rw - nfs 10.0.0.1:/x rw,vers=3,local_lock=flock,hard,nolock,proto=tcp,sec=sys,"
    "addr=10.0.0.1";
// Spaces in paths are escaped, and optional fields may precede the separator.
constexpr const char* nfs3Spaces =
    "595 515 0:66 / /mnt/with\\040space rw,relatime shared:1 master:2 - nfs 10.0.0.1:/a\\040b "
    "rw,vers=3,local_lock=posix";
constexpr const char* localFilesystem = "20 1 8:1 / / rw,relatime - ext4 /dev/sda1 rw,errors=remount-ro";
constexpr const char* notNfs          = "596 515 0:67 / /mnt/other rw - fuse.sshfs host:/ rw,local_lock=all";

std::optional<std::string> nfs_client_local_lock_option(const std::string& mountinfo, const std::string& device) {
    std::istringstream stream{mountinfo};
    return filesystem::nfs::local_lock_option(stream, device);
}

std::string joined(std::initializer_list<const char*> lines) {
    std::string text;
    for (const char* line : lines) {
        text += line;
        text += '\n';
    }
    return text;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("shared locking is not reported") {
    const std::string table = joined({localFilesystem, nfs4Shared, nfs3Shared, nfs3Flock});

    EXPECT(!nfs_client_local_lock_option(table, "0:60"));  // NFSv4
    EXPECT(!nfs_client_local_lock_option(table, "0:61"));  // NFSv3, local_lock=none
    EXPECT(!nfs_client_local_lock_option(table, "0:64"));  // local_lock=flock does not affect fcntl locks
    EXPECT(!nfs_client_local_lock_option(table, "8:1"));   // not NFS
    EXPECT(!nfs_client_local_lock_option(table, "0:99"));  // not listed
    EXPECT(!nfs_client_local_lock_option("", "0:60"));
}

CASE("client-local locking is reported with the offending option") {
    const std::string table = joined({nfs4Shared, nfs3NoLock, nfs3Posix, nfs3Legacy, nfs3Spaces});

    const auto noLock = nfs_client_local_lock_option(table, "0:62");
    EXPECT(noLock.has_value());
    EXPECT_EQUAL(*noLock, std::string("nolock"));

    const auto posix = nfs_client_local_lock_option(table, "0:63");
    EXPECT(posix.has_value());
    EXPECT_EQUAL(*posix, std::string("local_lock=posix"));

    const auto legacy = nfs_client_local_lock_option(table, "0:65");
    EXPECT(legacy.has_value());
    EXPECT_EQUAL(*legacy, std::string("nolock"));

    const auto spaces = nfs_client_local_lock_option(table, "0:66");
    EXPECT(spaces.has_value());
    EXPECT_EQUAL(*spaces, std::string("local_lock=posix"));
}

CASE("nolock is reported whatever local_lock= says") {
    const std::string table = joined({nfs3NoLockAndLocalLockNone, nfs3NoLockAndLocalLockFlock});

    const auto none = nfs_client_local_lock_option(table, "0:68");
    EXPECT(none.has_value());
    EXPECT_EQUAL(*none, std::string("nolock"));

    const auto flock = nfs_client_local_lock_option(table, "0:69");
    EXPECT(flock.has_value());
    EXPECT_EQUAL(*flock, std::string("nolock"));
}

CASE("only NFS mounts are considered and malformed lines are skipped") {
    const std::string table = joined({"garbage without a separator", " - ", notNfs, nfs3NoLock});

    EXPECT(!nfs_client_local_lock_option(table, "0:67"));  // local_lock=all, but not an NFS filesystem
    EXPECT(nfs_client_local_lock_option(table, "0:62").has_value());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}

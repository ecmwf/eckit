// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Metin Cakircali
/// @date   Oct 2026

#pragma once

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

namespace eckit::filesystem::nfs {

/// Finds the NFS mount option that makes fcntl locks local to this client, if any.
///
/// With `nolock` or `local_lock=posix|all` the kernel grants fcntl locks, including OFD locks, without asking the
/// server, so they do not exclude other clients. `local_lock=flock` only affects flock(2) and is not reported. Only
/// NFSv2/3 mounts can be in that state: the kernel clears these flags for NFSv4.
///
/// @param mountinfo Mount table in /proc/self/mountinfo format.
/// @param device    "major:minor" of the filesystem, as printed in the third column (see LocalPathName::device()).
/// @return The offending option, or nothing if locks are shared, the device is not an NFS mount, or it is not listed.
std::optional<std::string> local_lock_option(std::istream& mountinfo, std::string_view device);

/// As above, reading the mount table of this process. Linux only: nothing is reported elsewhere, or if
/// /proc/self/mountinfo cannot be read.
std::optional<std::string> local_lock_option(std::string_view device);

}  // namespace eckit::filesystem::nfs

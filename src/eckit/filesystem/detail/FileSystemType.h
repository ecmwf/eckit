/*
 * (C) Copyright 2026 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include <array>
#include <cstdint>

namespace eckit::filesystem::detail {

struct FileSystemMagic {
    uint32_t magic;
    const char* name;
};

inline constexpr std::array<FileSystemMagic, 20> file_system_magics = {{
    {0x6969, "nfs"},       {0xEF53, "ext"},       {0x58465342, "xfs"},     {0x9123683E, "btrfs"},
    {0x01021994, "tmpfs"}, {0x858458F6, "ramfs"}, {0x794C7630, "overlay"}, {0x0BD00BD0, "lustre"},
    {0x47504653, "gpfs"},  {0x00C36400, "ceph"},  {0x19830326, "beegfs"},  {0xAAD7AAEA, "panfs"},
    {0x65735546, "fuse"},  {0xFF534D42, "cifs"},  {0x2FC12FC1, "zfs"},     {0x73717368, "squashfs"},
    {0x9FA0, "proc"},      {0x62656572, "sysfs"}, {0x1CD1, "devpts"},      {0x6B414653, "afs"},
}};

constexpr const char* file_system_type_name(uint32_t magic) {
    for (const auto& known : file_system_magics) {
        if (known.magic == magic) {
            return known.name;
        }
    }
    return nullptr;
}

}  // namespace eckit::filesystem::detail

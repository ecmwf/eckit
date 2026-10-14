// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/NFSMountInfo.h"

#include <fstream>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace eckit::filesystem::nfs {

std::optional<std::string> local_lock_option(std::istream& mountinfo, std::string_view device) {

    // id parent major:minor root mountpoint options [optional fields] - fstype source super-options
    constexpr std::string_view separator = " - ";

    for (std::string line; std::getline(mountinfo, line);) {

        const auto split = line.find(separator);
        if (split == std::string::npos) {
            continue;
        }

        std::istringstream mount_fields{line.substr(0, split)};
        std::string id;
        std::string parent;
        std::string mount_device;
        mount_fields >> id >> parent >> mount_device;
        if (mount_device != device) {
            continue;
        }

        std::istringstream super_fields{line.substr(split + separator.size())};
        std::string type;
        std::string source;
        std::string super_options;
        super_fields >> type >> source >> super_options;
        if (type != "nfs" && type != "nfs4") {
            return std::nullopt;
        }

        std::istringstream options{super_options};
        for (std::string option; std::getline(options, option, ',');) {
            if (option == "nolock" || option == "local_lock=posix" || option == "local_lock=all") {
                return option;
            }
        }
        return std::nullopt;
    }

    return std::nullopt;
}

std::optional<std::string> local_lock_option([[maybe_unused]] std::string_view device) {
#ifdef __linux__
    std::ifstream mountinfo{"/proc/self/mountinfo"};
    return local_lock_option(mountinfo, device);
#else
    return std::nullopt;
#endif
}

}  // namespace eckit::filesystem::nfs

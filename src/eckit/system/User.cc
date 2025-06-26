/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#include "eckit/system/User.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"

#include <pwd.h>
#include <sys/types.h>

#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <mutex>
#include <string>

namespace {

/// Protects regions with access to password database.
/// Calls to setpwd / getpwuid / endpwent are not threadsafe.
class PwDbScope final {
public:

    PwDbScope() : lock(mtx) { setpwent(); }
    ~PwDbScope() { endpwent(); }

private:

    static std::mutex mtx;
    std::lock_guard<std::mutex> lock;
};

std::mutex PwDbScope::mtx{};

}  // namespace

namespace eckit::system {

UnknownUserId::UnknownUserId(const uid_t user_id) : Exception("No user found for uid:" + std::to_string(user_id)) {}

std::string current_user() {
    return user(getuid());
}

std::string user(const uid_t user_id) {
    PwDbScope lock{};
    errno = 0;
    do {
        const auto pw = getpwuid(user_id);
        if (pw != nullptr) {
            return pw->pw_name;
        }
    } while (errno == EINTR);
    const auto error = errno;
    if (error == 0) {
        throw UnknownUserId(user_id);
    }
    throw FailedSystemCall("getpwuid", Here(), error);
}
}  // namespace eckit::system

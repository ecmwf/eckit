// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/os/Password.h"

#include <pwd.h>

#include <cstring>

#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

bool Password::check(const std::string& user, const std::string& password) {
    struct passwd p;
    struct passwd* dummy;
    char line[1024];

    int n = getpwnam_r(user.c_str(), &p, line, sizeof(line), &dummy);

    if (n != 0) {
        Log::error() << "User " << user << " is unknown" << std::endl;
        return false;
    }

    bool match = password == p.pw_passwd;

    if (match) {
        Log::error() << "User " << user << " gave an valid password" << std::endl;
    }
    else {
        Log::error() << "User " << user << " gave an invalid password" << std::endl;
    }

    return match;
}

std::string Password::salt(const std::string& user) {
    struct passwd p;
    struct passwd* dummy;
    char line[1024];
    int n = getpwnam_r(user.c_str(), &p, line, sizeof(line), &dummy);

    if (n != 0) {
        Log::error() << "User " << user << " is unknown" << std::endl;
        return "";
    }

    char salt[3];
    ::strncpy(salt, p.pw_passwd, 2);
    salt[2] = 0;

    return salt;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

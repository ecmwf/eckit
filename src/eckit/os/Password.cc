/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <pwd.h>
#include <cstring>

#include "eckit/log/Log.h"
#include "eckit/os/Password.h"


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

    if (match)
        Log::error() << "User " << user << " gave an valid password" << std::endl;
    else
        Log::error() << "User " << user << " gave an invalid password" << std::endl;

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

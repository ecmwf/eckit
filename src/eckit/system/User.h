/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#pragma once

#include "eckit/exception/Exceptions.h"

#include <sys/types.h>

#include <string>


namespace eckit::system {

class UnknownUserId : public Exception {
public:

    explicit UnknownUserId(const uid_t user_id);
};

/// Reads the current users name from password database.
/// @return the user name
/// @throws FailedSystemCall if any system call failed.
/// @throws UnknownUserId if the current user id is not known to the password database
std::string current_user();

/// Reads the user name for a uid name from password database.
/// @param user_id of the user to look up.
/// @return the user name
/// @throws FailedSystemCall if any system call failed.
/// @throws UnknownUserId if the current user id is not known to the password database
std::string user(const uid_t user_id);

}  // namespace eckit::system

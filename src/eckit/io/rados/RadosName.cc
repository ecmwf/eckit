// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosName.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit::rados {

//----------------------------------------------------------------------------------------------------------------------

bool validName(const std::string& name) {

    static const std::string forbidden("/#?\0", 4);

    return name != "." && name != ".." && name.find_first_of(forbidden) == std::string::npos;
}

void checkName(const std::string& what, const std::string& name) {

    if (validName(name)) {
        return;
    }

    /// @note: a NUL byte would end the message early
    std::string printable = name;
    std::replace(printable.begin(), printable.end(), '\0', ' ');

    throw eckit::UserError("Invalid RADOS " + what + " name '" + printable +
                               "': it must not contain '/', '#', '?' or a NUL byte, nor be '.' or '..'",
                           Here());
}

std::vector<std::string> namesIn(const eckit::URI& uri, size_t count, const std::string& form) {

    std::vector<std::string> names;
    eckit::Tokenizer("/")(uri.name(), names);

    if (names.size() != count) {
        throw eckit::UserError("Invalid RADOS URI '" + uri.asString() + "', expected " + form, Here());
    }

    return names;
}

eckit::URI uriOf(const std::string& joined, const std::string& nspace) {

    if (nspace.empty()) {
        throw eckit::UserError("'" + joined + "' is in the default namespace, which a RADOS URI cannot express",
                               Here());
    }

    return eckit::URI{"rados", eckit::PathName(joined)};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::rados

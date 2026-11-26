// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/serialisation/BadTag.h"

namespace eckit {

BadTag::BadTag(const std::string& what) : Exception(what) {
    dumpStackTrace(std::cerr);
}

}  // namespace eckit

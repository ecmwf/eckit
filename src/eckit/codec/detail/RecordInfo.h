// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/codec/detail/Time.h"
#include "eckit/codec/detail/Version.h"

namespace eckit::codec {

struct RecordInfo {
    Version version_;
    Time created_;

    const Version& version() const { return version_; }
    const Time& created() const { return created_; }
};

}  // namespace eckit::codec

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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

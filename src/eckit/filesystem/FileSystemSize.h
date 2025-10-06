/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

namespace eckit {

//-----------------------------------------------------------------------------

struct FileSystemSize {
    unsigned long long available;
    unsigned long long total;
    FileSystemSize() : available(0), total(0) {}
};

//-----------------------------------------------------------------------------

}  // namespace eckit

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

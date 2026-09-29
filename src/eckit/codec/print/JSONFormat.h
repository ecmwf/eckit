// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <iosfwd>
#include <map>
#include <string>

#include "eckit/codec/Metadata.h"
#include "eckit/codec/Record.h"
#include "eckit/config/Configuration.h"

namespace eckit::codec {


class JSONFormat {
public:

    JSONFormat(const Record::URI& record, const Configuration&);

    void print(std::ostream&) const;

private:

    const Record record_;
    std::map<std::string, Metadata> items_;

    bool print_details_{false};
};


}  // namespace eckit::codec

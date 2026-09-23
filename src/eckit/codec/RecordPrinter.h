// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

#include "eckit/codec/Record.h"
#include "eckit/codec/Session.h"
#include "eckit/codec/detail/NoConfig.h"
#include "eckit/config/Configuration.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class RecordPrinter {
public:

    explicit RecordPrinter(const Record::URI&, const Configuration& = NoConfig());

    explicit RecordPrinter(const PathName&, const Configuration& = NoConfig());

    RecordPrinter(const PathName&, std::uint64_t offset, const Configuration& = NoConfig());

    Record record() const { return record_; }

    size_t size() const { return record_.size(); }

    Version version() const { return record_.version(); }

    Time time() const { return record_.time(); }

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream&, const RecordPrinter&);

private:

    Session session_;

    Record::URI uri_;

    struct {
        std::string format{"table"};
        bool details{false};
    } options_;

    Record record_;
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec

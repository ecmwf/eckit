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

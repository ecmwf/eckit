// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>

#include "eckit/codec/Metadata.h"
#include "eckit/codec/Record.h"
#include "eckit/codec/RecordItemReader.h"
#include "eckit/codec/Session.h"

namespace eckit::codec {

class MetadataPrettyPrintBase {
public:

    virtual ~MetadataPrettyPrintBase()      = default;
    virtual void print(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const MetadataPrettyPrintBase&);
    std::string str() const;
};

class MetadataPrettyPrint {
public:

    explicit MetadataPrettyPrint(const Metadata&);
    friend std::ostream& operator<<(std::ostream& out, const MetadataPrettyPrint& p);
    std::string str() const;

private:

    std::unique_ptr<MetadataPrettyPrintBase> impl_;
};

class TableFormat {
public:

    TableFormat(const Record::URI& record, const Parametrisation& config);

    void print(std::ostream&) const;

private:

    const Record record_;
    std::map<std::string, Metadata> items_;
    bool print_details_{false};
};


}  // namespace eckit::codec

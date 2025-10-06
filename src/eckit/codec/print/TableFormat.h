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

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

#include <cstddef>
#include <memory>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid {


class ICON final : public Unstructured {
public:
    // -- Types

    struct ICONRecord {
        explicit ICONRecord() = default;

        void read(const PathName&);

        using bytes_t = decltype(sizeof(int));
        bytes_t footprint() const;

        size_t n() const;

        std::vector<double> longitudes_;
        std::vector<double> latitudes_;
    };

    // -- Constructors

    explicit ICON(const Spec&);
    explicit ICON(uid_t);

    // -- Methods

    std::string arrangement() const;

    // -- Overridden methods

    uid_t calculate_uid() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

private:
    // -- Members

    uid_t uid_;
    Arrangement arrangement_;
    const ICONRecord& record_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
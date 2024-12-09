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

#include "eckit/stats/Statistics.h"
#include "eckit/stats/detail/Counter.h"


namespace eckit::stats::statistics {


/**
 * @brief Calculate entropy statistics on a Field
 */
class SimplePackingEntropy : public Statistics, detail::Counter {
public:
    // -- Constructors

    SimplePackingEntropy(const Parametrisation&);

    // -- Methods

    void reset();
    double entropy() const;
    double scale() const;
    size_t bucketCount() const;

private:
    // -- Members

    double entropy_;
    double scale_;
    size_t bucketCount_;

    // -- Overridden methods

    void execute(const Field&) override;
    void print(std::ostream&) const override;
};


}  // namespace eckit::stats::statistics

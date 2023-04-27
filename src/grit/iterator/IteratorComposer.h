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


#include <memory>
#include <vector>

#include "grit/Iterator.h"


namespace grit {
struct Projection;
struct Scanner;
}  // namespace grit


namespace grit::iterator {


class IteratorComposer final : public Iterator {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit IteratorComposer(Scanner*, const std::vector<Projection*>& = {});

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    bool operator++() override;
    bool operator++(int) override;

    // -- Methods

    size_t size() const override;
    const std::vector<std::unique_ptr<Projection>> projections() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::unique_ptr<Scanner> scanner_;
    std::vector<std::unique_ptr<Projection>> projections_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grit::iterator

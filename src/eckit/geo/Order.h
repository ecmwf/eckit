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

#include <memory>
#include <string>

#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Generator.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo::grid {
class Regular;
class Reduced;
}  // namespace eckit::geo::grid


namespace eckit::geo {


using Reordering = std::vector<size_t>;


class Order {
public:

    // -- Types

    using builder_t  = BuilderT1<Order>;
    using ARG1       = const Spec&;
    using value_type = std::string;

    // -- Constructors

    Order() noexcept    = default;
    Order(const Order&) = default;
    Order(Order&&)      = default;

    // -- Destructor

    virtual ~Order() = default;

    // -- Operators

    Order& operator=(const Order&) = default;
    Order& operator=(Order&&)      = default;

    // -- Methods

    [[nodiscard]] const Spec& spec() const;
    std::string spec_str() const { return spec().str(); }

    virtual const std::string& type() const = 0;

    virtual const value_type& order() const                = 0;
    virtual Reordering reorder(const value_type& to) const = 0;

    // -- Class methods

    static std::string className();

    [[nodiscard]] static value_type make_order_from_spec(const Spec&);
    [[nodiscard]] static Reordering no_reorder(size_t);

protected:

    // -- Class methods

    static void register_ordering(const std::string&);

private:

    // -- Members

    mutable std::shared_ptr<spec::Custom> spec_;

    // -- Methods

    virtual void fill_spec(spec::Custom&) const = 0;

    // -- Friends

    friend class grid::Reduced;
    friend class grid::Regular;
};


using OrderFactoryType = Factory<Order>;


template <typename T>
using OrderRegisterType = ConcreteBuilderT1<Order, T>;


struct OrderFactory {
    [[nodiscard]] static const Order* build(const Spec& spec) { return instance().make_from_spec_(spec); }

    [[nodiscard]] static const Order* make_from_string(const std::string&);
    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }

    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static OrderFactory& instance();

    [[nodiscard]] const Order* make_from_spec_(const Spec&) const;
    [[nodiscard]] Spec* make_spec_(const Spec&) const;

    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo

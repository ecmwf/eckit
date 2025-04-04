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


namespace eckit::geo {


using Reorder = std::vector<size_t>;


class Ordering {
public:

    // -- Types

    using builder_t = BuilderT1<Ordering>;
    using ARG1      = const Spec&;

    enum ordering_type {
        scan_i_positively_j_negatively_ij_i_single_direction,
        scan_i_negatively_j_negatively_ij_i_single_direction,
        scan_i_positively_j_positively_ij_i_single_direction,
        scan_i_negatively_j_positively_ij_i_single_direction,
        scan_i_positively_j_negatively_ji_i_single_direction,
        scan_i_negatively_j_negatively_ji_i_single_direction,
        scan_i_positively_j_positively_ji_i_single_direction,
        scan_i_negatively_j_positively_ji_i_single_direction,
        scan_i_positively_j_negatively_ij_i_alternating_direction,
        scan_i_negatively_j_negatively_ij_i_alternating_direction,
        scan_i_positively_j_positively_ij_i_alternating_direction,
        scan_i_negatively_j_positively_ij_i_alternating_direction,
        scan_i_positively_j_negatively_ji_i_alternating_direction,
        scan_i_negatively_j_negatively_ji_i_alternating_direction,
        scan_i_positively_j_positively_ji_i_alternating_direction,
        scan_i_negatively_j_positively_ji_i_alternating_direction,
        // TODO regular_ ... shift

        healpix_ring,
        healpix_nested,

        scan_ordering     = scan_i_positively_j_negatively_ij_i_single_direction,
        scan_ordering_end = scan_i_negatively_j_positively_ji_i_alternating_direction,

        healpix_ordering     = healpix_ring,
        healpix_ordering_end = healpix_nested,
    };

    // -- Constructors

    Ordering() noexcept       = default;
    Ordering(const Ordering&) = default;
    Ordering(Ordering&&)      = default;

    // -- Destructor

    virtual ~Ordering() = default;

    // -- Operators

    Ordering& operator=(const Ordering&) = default;
    Ordering& operator=(Ordering&&)      = default;

    // -- Methods

    virtual const std::string& type() const = 0;

    virtual Ordering::ordering_type to_type() const   = 0;
    virtual Ordering::ordering_type from_type() const = 0;
    virtual Reorder reorder() const                   = 0;

    static bool is_scan_i_positive(Ordering::ordering_type);
    static bool is_scan_j_positive(Ordering::ordering_type);
    static bool is_scan_ij(Ordering::ordering_type);
    static bool is_scan_alternating_direction(Ordering::ordering_type);

    [[nodiscard]] static Ordering::ordering_type make_ordering_from_spec(const Spec&);

    // -- Class methods

    static std::string className() { return "ordering"; }

private:

    // -- Members

    mutable std::shared_ptr<spec::Custom> spec_;

    // -- Methods

    virtual void fill_spec(spec::Custom&) const = 0;
};


using OrderingFactoryType = Factory<Ordering>;
using OrderingSpecByName  = spec::GeneratorT<spec::SpecGeneratorT1<const std::string&>>;


template <typename T>
using OrderingRegisterType = ConcreteBuilderT1<Ordering, T>;

template <typename T>
using OrderingRegisterName = spec::ConcreteSpecGeneratorT1<T, const std::string&>;


struct OrderingFactory {
    [[nodiscard]] static const Ordering* build(const Spec& spec) { return instance().make_from_spec_(spec); }

    [[nodiscard]] static const Ordering* make_from_string(const std::string&);
    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }

    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static OrderingFactory& instance();

    [[nodiscard]] const Ordering* make_from_spec_(const Spec&) const;
    [[nodiscard]] Spec* make_spec_(const Spec&) const;

    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo

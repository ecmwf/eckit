// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/memory/Builder.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Generator.h"
#include "eckit/spec/Spec.h"


namespace eckit {
class JSON;
namespace geo {
class Area;
class Range;
class Search;
}  // namespace geo
}  // namespace eckit


namespace eckit::geo {


class Grid {
public:

    // -- Types

    using uid_type      = std::string;
    using order_type    = std::string;
    using renumber_type = std::vector<size_t>;

    using BoundingBox = area::BoundingBox;

    using builder_t = BuilderT1<Grid>;
    using Spec      = spec::Spec;
    using ARG1      = const Spec&;

    /// Position in a grid, by index (the end is the grid size)
    class Iterator final {
    public:

        /// What a grid remembers between points, besides the index (e.g. the current row)
        struct State {
            virtual ~State()                           = default;
            virtual Point point(size_t index)          = 0;
            [[nodiscard]] virtual State* clone() const = 0;
        };

        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Point;
        using pointer           = value_type*;
        using reference         = value_type&;

        explicit Iterator(const Grid&, bool end = false);

        Iterator(const Iterator&);
        Iterator(Iterator&&) noexcept = default;

        ~Iterator() = default;

        Iterator& operator=(const Iterator&);
        Iterator& operator=(Iterator&&) noexcept = default;

        bool operator==(const Iterator& other) const { return index_ == other.index_; }
        bool operator!=(const Iterator& other) const { return index_ != other.index_; }

        bool operator++() { return operator+=(1); }
        bool operator--() { return operator+=(-1); }
        bool operator+=(difference_type);
        bool operator-=(difference_type d) { return operator+=(-d); }

        explicit operator bool() const { return index_ < size_; }
        Point operator*() const { return state_ ? state_->point(index_) : grid_->point(index_); }

        size_t index() const { return index_; }

    private:

        const Grid* grid_;
        size_t index_;
        size_t size_;
        std::unique_ptr<State> state_;
    };

    // -- Constructors

    Grid(const Grid&) = delete;
    Grid(Grid&&)      = delete;

    // -- Destructor

    virtual ~Grid();

    // -- Operators

    Grid& operator=(const Grid&) = delete;
    Grid& operator=(Grid&&)      = delete;

    // -- Methods

    Iterator begin() const { return cbegin(); }
    Iterator end() const { return cend(); }

    Iterator cbegin() const { return Iterator{*this}; }
    Iterator cend() const { return Iterator{*this, true}; }

    /// Point by index, in the grid order; iterating is faster over many points
    [[nodiscard]] virtual Point point(size_t index) const = 0;

    [[nodiscard]] const Spec& spec() const { return custom_spec(); }
    [[nodiscard]] std::string spec_str() const { return spec().str(); }

    [[nodiscard]] const spec::Spec& catalog() const;
    [[nodiscard]] std::string catalog_str() const { return catalog().str(); }

    virtual const std::string& type() const   = 0;
    virtual std::vector<size_t> shape() const = 0;

    /// Name and staggering arrangement, for the grids that are identified by them (eg. ORCA, FESOM, ICON)
    virtual std::string name() const { return {}; }
    virtual std::string arrangement() const { return {}; }

    virtual bool empty() const;
    virtual size_t size() const;
    virtual void cache() const;

    uid_type uid() const;
    [[nodiscard]] virtual uid_type calculate_uid() const;

    [[nodiscard]] Point first_point() const;
    [[nodiscard]] Point last_point() const;
    [[nodiscard]] virtual std::vector<Point> to_points() const;

    /// Coordinate names of the grid, eg. ("x", "y") or ("longitude", "latitude") (see point_coordinates)
    const std::vector<std::string>& point_coordinates() const;

    /// Coordinates (one vector per coordinate), on their figure, as geodetic (lat, lon)
    [[nodiscard]] virtual std::pair<std::vector<double>, std::vector<double>> to_latlons() const;

    /// Coordinates (one vector per coordinate), on their figure, as geocentric cartesian (x, y, z)
    [[nodiscard]] virtual std::vector<std::vector<double>> to_xyz() const;

    /// Convert the grid to an unstructured grid (coordinates are cached, exchangeable by uid)
    [[nodiscard]] Grid* to_unstructured_ll(const std::string& name = "") const;

    /// Shared (cache::SearchCache), valid for the grid lifetime
    const Search& search() const;

    virtual size_t truncation() const;

    virtual const order_type& order() const;
    virtual renumber_type reorder(const order_type&) const;

    virtual const Area& area() const;
    virtual renumber_type crop(const Area&) const;

    virtual const Projection& projection() const;
    /// Figure of the grid points (lon, lat), as the projection's
    virtual const Figure& figure() const { return projection().figure(); }

    virtual const BoundingBox& boundingBox() const;
    [[nodiscard]] virtual BoundingBox* calculate_bbox() const;

    [[nodiscard]] virtual Grid* make_grid_reordered(const order_type&) const;
    /// The grid cropped to the area, of the same type if supported (e.g. regular_ll by a bounding box), otherwise
    /// unstructured (see make_grid_cropped_unstructured_ll)
    [[nodiscard]] virtual Grid* make_grid_cropped(const Area&) const;

    /// The points in the area (see crop), as an unstructured grid (its coordinates cached, exchangeable by uid)
    [[nodiscard]] Grid* make_grid_cropped_unstructured_ll(const Area&) const;

    double dx() const;
    double dy() const;
    virtual size_t nx() const;
    virtual size_t ny() const;
    virtual const Range& x() const;
    virtual const Range& y() const;

    double dlon() const;
    double dlat() const;
    size_t nlon() const;
    size_t nlat() const;
    virtual const Range& lon() const;
    virtual const Range& lat() const;

    // -- Class methods

    static std::string className() { return "grid"; }
    static bool is_uid(const std::string&);

protected:

    // -- Constructors

    explicit Grid(BoundingBox* = nullptr, const Projection* = nullptr);

    // -- Methods

    virtual void fill_spec(spec::Custom&) const;

    /// What iterating over this grid remembers between points, if anything
    [[nodiscard]] virtual Iterator::State* make_state() const { return nullptr; }

    void reset_uid(uid_type = {});

    void projection(const Projection* ptr) { projection_.reset(ptr); }
    void boundingBox(BoundingBox* bbox) { bbox_.reset(bbox); }

    [[nodiscard]] static BoundingBox* bounding_box_from_spec(const Spec&);

private:

    // -- Members

    mutable std::unique_ptr<const BoundingBox> bbox_;
    mutable std::unique_ptr<const Projection> projection_;
    mutable std::unique_ptr<const Spec> catalog_;
    mutable std::unique_ptr<spec::Custom> spec_;
    mutable uid_type uid_;

    mutable std::shared_ptr<const Search> search_;

    struct Once;
    const std::unique_ptr<Once> once_;

    // -- Methods

    const spec::Custom& custom_spec() const;

    // -- Friends

    friend bool operator==(const Grid& a, const Grid& b) { return a.spec_str() == b.spec_str(); }
    friend bool operator!=(const Grid& a, const Grid& b) { return !(a == b); }
};


struct GridSpecByName {
    using key_t                = std::string;
    using generator_t          = spec::GeneratorT<spec::SpecGeneratorT1<const key_t&>>;
    using concrete_generator_t = generator_t::generator_t;

    static generator_t& instance();

    /// Register a name pattern (a regular expression, case-insensitive)
    static void regist(const key_t& pattern, concrete_generator_t* gen) {
        generator_t::instance().regist(pattern, gen);
    }
    static void unregist(const key_t& pattern) { generator_t::instance().unregist(pattern); }

    /// Register a name (matched literally, case-insensitive) with its spec, or replace it; these don't load the shared
    /// grids (as instance does), since loading them registers by these
    static void regist(const key_t& name, const spec::Custom&);
    static void replace(const key_t& name, const spec::Custom&);

    /// If a name is registered (literally, not as matching a pattern)
    static bool has_name(const std::string& name);
};


struct GridSpecByUID {
    using key_t                = Grid::uid_type;
    using generator_t          = spec::GeneratorT<spec::SpecGeneratorT0>;
    using concrete_generator_t = generator_t::generator_t;

    static generator_t& instance();
    static void regist(const key_t& key, concrete_generator_t* gen) { generator_t::instance().regist(key, gen); }
    static void unregist(const key_t& key) { generator_t::instance().unregist(key); }

    /// Register a uid (case-insensitive) with its spec, or replace it; these don't load the shared grids (as instance
    /// does), since loading them registers by these
    static void regist(const key_t& key, const spec::Custom&);
    static void replace(const key_t& key, const spec::Custom&);
};


template <typename T>
using GridRegisterType = ConcreteBuilderT1<Grid, T>;


template <typename T>
using GridRegisterUID = spec::ConcreteSpecGeneratorT0<T>;


template <typename T>
struct GridRegisterName {
    explicit GridRegisterName(const std::string& name_or_pattern) {
        new spec::ConcreteSpecGeneratorT1<T, const std::string&>(name_or_pattern);
    }
};


struct GridFactory {
    // This is 'const' as Grid should always be immutable
    [[nodiscard]] static const Grid* build(const Grid::Spec&);
    [[nodiscard]] static const Grid* make_from_string(const std::string&);

    [[nodiscard]] static Grid::Spec* make_spec(const Grid::Spec&);
    static std::ostream& list(std::ostream&);
};


}  // namespace eckit::geo

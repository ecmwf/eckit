// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Projection.h"

#include <array>
#include <limits>
#include <ostream>
#include <utility>
#include <variant>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/projection/EquidistantCylindrical.h"
#include "eckit/geo/projection/None.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/share/Projection.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Layered.h"
#include "eckit/types/FloatCompare.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


/// Tag carrying a point type, see visit_point_type
template <typename P>
struct point_tag {
    using type = P;
};


/// Call f(point_tag<P>{}) for the point type P, the Point alternative at the given index (not the empty point)
template <typename F, size_t I = 1>
void visit_point_type(size_t index, F&& f) {
    if constexpr (I < std::variant_size_v<Point>) {
        if (index == I) {
            f(point_tag<std::variant_alternative_t<I, Point>>{});
            return;
        }
        visit_point_type<F, I + 1>(index, std::forward<F>(f));
    }
    else {
        throw BadValue("Projection: invalid point type (" + std::to_string(index) + ")", Here());
    }
}


const std::vector<std::string>& coordinates(size_t index) {
    const std::vector<std::string>* names = nullptr;
    visit_point_type(index, [&names](auto tag) { names = &point_coordinates<typename decltype(tag)::type>(); });
    return *names;
}


size_t dims(size_t index) {
    size_t n = 0;
    visit_point_type(index, [&n](auto tag) { n = decltype(tag)::type::DIMS; });
    return n;
}


/// Check the vectors of coordinates of points of a point type (v3 is empty for points of 2 coordinates)
void check_vectors(const std::vector<double>& v1, const std::vector<double>& v2, const std::vector<double>& v3,
                   size_t type) {
    const auto n = v1.size();
    if (dims(type) == 3 ? v2.size() == n && v3.size() == n : v2.size() == n && v3.empty()) {
        return;
    }

    std::string names;
    for (const auto& name : coordinates(type)) {
        names += (names.empty() ? "" : ", ") + name;
    }

    throw BadValue("Projection: expected coordinates (" + names + ") of the same size, got sizes " +
                       std::to_string(v1.size()) + ", " + std::to_string(v2.size()) + ", " + std::to_string(v3.size()),
                   Here());
}


/// Project points one by one, from/to point types known only at run time (resolved once); failures are NaN
template <typename F>
std::vector<std::vector<double>> project_points(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3, size_t from, size_t to, F project) {
    const std::array<const std::vector<double>*, 3> in{&v1, &v2, &v3};
    const auto n = v1.size();

    std::vector<std::vector<double>> out(dims(to), std::vector<double>(n));

    visit_point_type(from, [&](auto from_tag) {
        visit_point_type(to, [&](auto to_tag) {
            using P = typename decltype(from_tag)::type;
            using Q = typename decltype(to_tag)::type;

            for (size_t i = 0; i < n; ++i) {
                P p;
                for (size_t k = 0; k < P::DIMS; ++k) {
                    p.data()[k] = (*in[k])[i];
                }

                try {
                    const auto q  = project(Point{p});
                    const auto& r = std::get<Q>(q);  // throws if the projection doesn't honour its point types
                    for (size_t k = 0; k < Q::DIMS; ++k) {
                        out[k][i] = r.data()[k];
                    }
                }
                catch (const eckit::Exception&) {
                    for (auto& o : out) {
                        o[i] = std::numeric_limits<double>::quiet_NaN();
                    }
                }
            }
        });
    });

    return out;
}


}  // namespace


Projection::Projection(Figure* ptr, const Point& source, const Point& target) :
    figure_(ptr != nullptr ? ptr : FigureFactory::make_default()) {
    ASSERT(figure_);
    point_types(source, target);
}


std::vector<std::vector<double>> Projection::fwd(const std::vector<double>& v1, const std::vector<double>& v2,
                                                 const std::vector<double>& v3) const {
    check_vectors(v1, v2, v3, source_);
    return fwd_vector(v1, v2, v3);
}


std::vector<std::vector<double>> Projection::inv(const std::vector<double>& v1, const std::vector<double>& v2,
                                                 const std::vector<double>& v3) const {
    check_vectors(v1, v2, v3, target_);
    return inv_vector(v1, v2, v3);
}


const std::vector<std::string>& Projection::source_point_coordinates() const {
    return coordinates(source_);
}


const std::vector<std::string>& Projection::target_point_coordinates() const {
    return coordinates(target_);
}


void Projection::point_types(const Point& source, const Point& target) {
    // validates the point types (the empty point is not a point type)
    dims(source.index());
    dims(target.index());

    source_ = source.index();
    target_ = target.index();
}


void Projection::point_types_from(const Projection& first, const Projection& last) {
    source_ = first.source_;
    target_ = last.target_;
}


void Projection::reverse_point_types() {
    std::swap(source_, target_);
}


std::vector<std::vector<double>> Projection::fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const {
    return project_points(v1, v2, v3, source_, target_, [this](const Point& p) { return fwd(p); });
}


std::vector<std::vector<double>> Projection::inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const {
    return project_points(v1, v2, v3, target_, source_, [this](const Point& q) { return inv(q); });
}


const Projection::Spec& Projection::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();

        auto& custom = *spec_;
        fill_spec(custom);

        if (std::string name; !custom.empty() && ProjectionSpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set(className(), name);
        }
    }

    return *spec_;
}


std::string Projection::proj_str() const {
#if eckit_HAVE_PROJ
    return projection::PROJ::proj_str(dynamic_cast<const spec::Custom&>(spec()));
#else
    NOTIMP;
#endif
}


bool Projection::is_default() const {
    return *this == projection_default();
}


const Projection& Projection::projection_default() {
    static const projection::None PROJECTION;
    return PROJECTION;
}


const Projection* ProjectionFactory::make_default() {
    return ProjectionFactory::build(Projection::projection_default().spec());
}


void Projection::fill_spec(spec::Custom& custom) const {
    if (!figure_->is_default()) {
        figure_->fill_spec(custom);
    }

    if (!types::is_approximately_equal(false_.X(), 0.)) {
        custom.set("x_0", false_.X());
    }

    if (!types::is_approximately_equal(false_.Y(), 0.)) {
        custom.set("y_0", false_.Y());
    }
}


const Projection* ProjectionFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Projection::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


ProjectionFactory& ProjectionFactory::instance() {
    static ProjectionFactory obj;
    return obj;
}


const Projection* ProjectionFactory::make_from_spec_(const Projection::Spec& spec) const {
    lock_type lock;

    // an explicit 'projection' has to name its type
    if (spec.has("projection")) {
        const auto& cfg = spec.spec("projection");
        return ProjectionFactoryType::instance().get(cfg.get_string("type")).create(cfg);
    }

    if (spec.has("rotation")) {
        if (auto* rotation = projection::Rotation::make_from_spec(spec); rotation != nullptr) {
            return rotation;
        }
    }

    std::unique_ptr<Projection::Spec> cfg(make_spec_(spec));

    // NOTE: a 'type' that isn't a projection's is somebody else's (eg. a grid's)
    if (std::string type; cfg->get("type", type) && has_type(type)) {
        return ProjectionFactoryType::instance().get(type).create(*cfg);
    }

    return new projection::EquidistantCylindrical(*cfg);
}


Projection::Spec* ProjectionFactory::make_spec_(const Projection::Spec& spec) const {
    lock_type lock;
    share::Projection::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to projectionspec)

    if (spec.has("proj")) {
        cfg->push_back(new spec::Custom{{"type", "proj"}});
    }
    else if (spec.has("rotation")) {
        std::vector<double> rotation;
        spec.get("rotation", rotation);
        cfg->push_back(new spec::Custom{{"type", "rotation"}, {"rotation", rotation}});
    }

    return cfg;
}


std::ostream& ProjectionFactory::list_(std::ostream& out) const {
    lock_type lock;
    share::Projection::instance();

    out << ProjectionSpecByName::instance() << std::endl;
    out << ProjectionFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo

# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector


cdef extern from "eckit/geo/Grid.h" namespace "eckit::geo":
    cdef cppclass Grid:
        pass


cdef extern from * namespace "eckit::geo::python":
    """
    #include <memory>
    #include <sstream>
    #include <string>
    #include <utility>
    #include <vector>

    #include "eckit/geo/Exceptions.h"
    #include "eckit/geo/Grid.h"
    #include "eckit/geo/Point.h"
    #include "eckit/geo/Search.h"
    #include "eckit/geo/cache/SearchCache.h"
    #include "eckit/parser/YAMLParser.h"
    #include "eckit/spec/Custom.h"

    namespace eckit::geo::python {

    using CSR   = Search::CSR;
    using Point = geo::Point;

    // indices and distances of the neighbours of a point
    struct Result {
        std::vector<size_t> index;
        std::vector<double> distance;
    };

    // a search, kept in use (if cached, not evicted) while referenced
    struct SearchHandle {
        std::shared_ptr<const Search> search;
    };

    using handle_t = SearchHandle;

    inline std::unique_ptr<spec::Custom> to_spec(const std::string& spec) {
        return std::unique_ptr<spec::Custom>(
            spec::Custom::make_from_value(YAMLParser::decodeString(spec)));
    }

    inline SearchHandle* search_get(const Grid& grid, const std::string& spec) {
        return new SearchHandle{cache::SearchCache::instance().get(grid, *to_spec(spec))};
    }

    inline SearchHandle* search_from_xyz(
        size_t n, const double* x, const double* y, const double* z,
        const std::string& spec) {
        std::vector<PointXYZ> points;
        points.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            points.emplace_back(x[i], y[i], z[i]);
        }
        return new SearchHandle{std::make_shared<const Search>(points, *to_spec(spec))};
    }

    // another behaviour, sharing the k-d tree (keeping the other search in use, as
    // cache::SearchCache)
    inline SearchHandle* search_configured(const handle_t& h, const std::string& spec) {
        return new SearchHandle{std::shared_ptr<const Search>(
            new Search(*h.search, *to_spec(spec)),
            [keep = h.search](const Search* ptr) { delete ptr; })};
    }

    // (lon, lat), (x, y, z), or (lon, lat) and height
    inline Point make_point(const std::vector<double>& c, bool height) {
        if (height && c.size() == 3) {
            return PointLonLatH{c[0], c[1], c[2]};
        }
        if (!height && c.size() == 2) {
            return PointLonLat{c[0], c[1]};
        }
        if (!height && c.size() == 3) {
            return PointXYZ{c[0], c[1], c[2]};
        }
        throw exception::SearchError(
            "Search: expected point (lon, lat) or (x, y, z), or (lon, lat) and height",
            Here());
    }

    inline Result to_result(const Search::Neighbours& v) {
        Result r;
        r.index.reserve(v.size());
        r.distance.reserve(v.size());
        for (const auto& n : v) {
            r.index.push_back(n.index);
            r.distance.push_back(n.distance);
        }
        return r;
    }

    inline Result search_point(const handle_t& h, const Point& p) {
        return to_result(h.search->search(p));
    }

    inline Result search_nn(const handle_t& h, const Point& p) {
        return to_result({h.search->search_nn(p)});
    }

    inline Result search_knn(const handle_t& h, const Point& p, size_t k) {
        return to_result(h.search->search_knn(p, k));
    }

    inline Result search_radius(const handle_t& h, const Point& p, double r) {
        return to_result(h.search->search_radius(p, r));
    }

    inline Result search_knn_or_radius(
        const handle_t& h, const Point& p, size_t k, double r) {
        return to_result(h.search->search_knn_or_radius(p, k, r));
    }

    inline Result search_knn_and_radius(
        const handle_t& h, const Point& p, size_t k, double r) {
        return to_result(h.search->search_knn_and_radius(p, k, r));
    }

    inline Result search_knn_or_same_radius(
        const handle_t& h, const Point& p, size_t k, double tolerance) {
        return to_result(h.search->search_knn_or_same_radius(p, k, tolerance));
    }

    inline Search::ColumnOrder column_order(bool by_index) {
        return by_index ? Search::ColumnOrder::INDEX : Search::ColumnOrder::DISTANCE;
    }

    inline CSR csr_search_xyz(
        const handle_t& h, size_t n, const double* x, const double* y, const double* z,
        bool by_index) {
        std::vector<PointXYZ> points;
        points.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            points.emplace_back(x[i], y[i], z[i]);
        }
        return h.search->csr_search(points, column_order(by_index));
    }

    inline CSR csr_search_ll(
        const handle_t& h, size_t n, const double* lon, const double* lat,
        bool by_index) {
        std::vector<PointLonLat> points;
        points.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            points.emplace_back(lon[i], lat[i]);
        }
        return h.search->csr_search(points, column_order(by_index));
    }

    inline CSR csr_search_llh(
        const handle_t& h, size_t n, const double* lon, const double* lat,
        const double* height, bool by_index) {
        std::vector<PointLonLatH> points;
        points.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            points.emplace_back(lon[i], lat[i], height[i]);
        }
        return h.search->csr_search(points, column_order(by_index));
    }

    inline std::pair<size_t, size_t> search_estimate(size_t size) {
        const auto f = Search::estimate(size);
        return {f.processMemory(), f.sharedMemory()};
    }

    inline size_t search_size(const SearchHandle& h) {
        return h.search->size();
    }

    inline std::pair<size_t, size_t> search_footprint(const SearchHandle& h) {
        const auto f = h.search->footprint();
        return {f.processMemory(), f.sharedMemory()};
    }

    inline std::string search_str(const SearchHandle& h) {
        std::ostringstream str;
        str << *h.search;
        return str.str();
    }

    }
    """
    cdef cppclass SearchHandle:
        pass

    cdef cppclass Point:
        pass

    cdef cppclass Result:
        vector[size_t] index
        vector[double] distance

    cdef cppclass CSR:
        size_t rows()
        size_t size()
        size_t cols
        vector[int] offsets
        vector[int] indices
        vector[double] distances

    SearchHandle* search_get(const Grid&, const string& spec) except +
    SearchHandle* search_from_xyz(
        size_t, const double*, const double*, const double*, const string& spec
    ) except + nogil
    SearchHandle* search_configured(const SearchHandle&, const string& spec) except +

    Point make_point(const vector[double]&, bint height) except +

    Result search_point(const SearchHandle&, const Point&) except +
    Result search_nn(const SearchHandle&, const Point&) except +
    Result search_knn(const SearchHandle&, const Point&, size_t) except +
    Result search_radius(const SearchHandle&, const Point&, double) except +
    Result search_knn_or_radius(
        const SearchHandle&, const Point&, size_t, double
    ) except +
    Result search_knn_and_radius(
        const SearchHandle&, const Point&, size_t, double
    ) except +
    Result search_knn_or_same_radius(
        const SearchHandle&, const Point&, size_t, double
    ) except +

    CSR csr_search_xyz(
        const SearchHandle&, size_t, const double*, const double*, const double*, bint
    ) except + nogil
    CSR csr_search_ll(
        const SearchHandle&, size_t, const double*, const double*, bint
    ) except + nogil
    CSR csr_search_llh(
        const SearchHandle&, size_t, const double*, const double*, const double*, bint
    ) except + nogil

    pair[size_t, size_t] search_estimate(size_t) except +

    size_t search_size(const SearchHandle&) except +
    pair[size_t, size_t] search_footprint(const SearchHandle&) except +
    string search_str(const SearchHandle&) except +

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

    using neighbour_t  = std::pair<size_t, double>;
    using neighbours_t = std::vector<neighbour_t>;
    using point_t      = std::vector<double>;

    // a cached search, kept in use (not evicted) while referenced
    struct SearchHandle {
        std::shared_ptr<const Search> search;
    };

    using handle_t = SearchHandle;

    inline SearchHandle* search_get(const Grid& grid, const std::string& spec) {
        std::unique_ptr<spec::Custom> custom(
            spec::Custom::make_from_value(YAMLParser::decodeString(spec)));
        return new SearchHandle{cache::SearchCache::instance().get(grid, *custom)};
    }

    inline Point to_point(const point_t& c) {
        if (c.size() == 2) {
            return PointLonLat{c[0], c[1]};
        }
        if (c.size() == 3) {
            return PointXYZ{c[0], c[1], c[2]};
        }
        throw exception::SearchError(
            "Search: expected point (lon, lat) or (x, y, z)", Here());
    }

    inline neighbour_t to_neighbour(const Search::Neighbour& n) {
        return {n.index, n.distance};
    }

    inline neighbours_t to_neighbours(const Search::Neighbours& v) {
        neighbours_t result;
        result.reserve(v.size());
        for (const auto& n : v) {
            result.emplace_back(to_neighbour(n));
        }
        return result;
    }

    inline neighbours_t search_point(const handle_t& h, const point_t& p) {
        return to_neighbours(h.search->search(to_point(p)));
    }

    inline neighbour_t search_nn(const handle_t& h, const point_t& p) {
        return to_neighbour(h.search->search_nn(to_point(p)));
    }

    inline neighbours_t search_knn(const handle_t& h, const point_t& p, size_t k) {
        return to_neighbours(h.search->search_knn(to_point(p), k));
    }

    inline neighbours_t search_radius(const handle_t& h, const point_t& p, double r) {
        return to_neighbours(h.search->search_radius(to_point(p), r));
    }

    inline neighbours_t search_knn_or_radius(
        const handle_t& h, const point_t& p, size_t k, double r) {
        return to_neighbours(h.search->search_knn_or_radius(to_point(p), k, r));
    }

    inline neighbours_t search_knn_and_radius(
        const handle_t& h, const point_t& p, size_t k, double r) {
        return to_neighbours(h.search->search_knn_and_radius(to_point(p), k, r));
    }

    inline size_t search_size(const SearchHandle& h) {
        return h.search->size();
    }

    inline std::pair<size_t, size_t> search_footprint(const SearchHandle& h) {
        const auto f = h.search->footprint();
        return {f.memory(), f.shared()};
    }

    inline std::string search_str(const SearchHandle& h) {
        std::ostringstream str;
        str << *h.search;
        return str.str();
    }

    inline std::vector<size_t> search_cache_statistics() {
        const auto& cache = cache::SearchCache::instance();
        const auto stats  = cache.statistics();
        const auto usage  = cache.usage();
        return {stats.hits, stats.misses, stats.evictions, cache.size(),
                usage.memory(), usage.shared()};
    }

    inline void search_cache_clear() {
        cache::SearchCache::instance().clear();
    }

    }
    """
    cdef cppclass SearchHandle:
        pass

    SearchHandle* search_get(const Grid&, const string& spec) except +

    vector[pair[size_t, double]] search_point(
        const SearchHandle&, const vector[double]&
    ) except +
    pair[size_t, double] search_nn(const SearchHandle&, const vector[double]&) except +
    vector[pair[size_t, double]] search_knn(
        const SearchHandle&, const vector[double]&, size_t
    ) except +
    vector[pair[size_t, double]] search_radius(
        const SearchHandle&, const vector[double]&, double
    ) except +
    vector[pair[size_t, double]] search_knn_or_radius(
        const SearchHandle&, const vector[double]&, size_t, double
    ) except +
    vector[pair[size_t, double]] search_knn_and_radius(
        const SearchHandle&, const vector[double]&, size_t, double
    ) except +

    size_t search_size(const SearchHandle&) except +
    pair[size_t, size_t] search_footprint(const SearchHandle&) except +
    string search_str(const SearchHandle&) except +

    vector[size_t] search_cache_statistics() except +
    void search_cache_clear() except +

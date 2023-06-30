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


#include "eccodes.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/grid/grib/GribConfiguration.h"


#if 0
(grib2/template.3.(gaussian|grid|latlon).def)
  double latitudeOfFirstPointInDegrees
  double latitudeOfLastPointInDegrees
  double longitudeOfFirstPointInDegrees
  double longitudeOfLastPointInDegrees
  double DiInDegrees
  double DjInDegrees
  size_t n_i
  size_t n_j
  size_t numberOfParallelsBetweenAPoleAndTheEquator
  size_t basicAngleOfTheInitialProductionDomain
  size_t subdivisionsOfBasicAngle
#endif


#if 0
eckit::grid::Figure* make_figure(long code) {
    // Code table 3.2 – Shape of the reference system

    switch (code) {
        case 0:
            return new eckit::grid::figure::Sphere(6367470.);
        case 1:
            // Earth assumed spherical with radius specified (in m) by data producer
            NOTIMP;
        case 2:
            return new eckit::grid::figure::Spheroid(6378160., 6356775.);
        case 3:
            // Earth assumed oblate spheroid with major and minor axes specified (in km) by data producer
            NOTIMP;
        case 4:
            return new eckit::grid::figure::Spheroid(6378137., 6356752.314);
        case 5:
            return new eckit::grid::figure::Spheroid(6378137., 6356752.314140347);
        case 6:
            return new eckit::grid::figure::Sphere(6371229.);
        case 7:
            // Earth assumed oblate spheroid with major or minor axes specified (in m) by data producer
            NOTIMP;
        case 8:
            return new eckit::grid::figure::Sphere(6371200.);
        case 9:
            return new eckit::grid::figure::Spheroid(6377563.396, 6356256.909);
        case 10:
            // Earth model assumed WGS84 with corrected geomagnetic coordinates (latitude and longitude) defined by
            // Gustafsson et al., 1992
            NOTIMP;
        case 11:
            return new eckit::grid::figure::Sphere(695990000.);
        default:
            NOTIMP;
    }
}
#endif


#if 0
    std::string type() const { return get_string("gridType"); }
    
    size_t Ni() const { return get_size_t("Ni"); }
    size_t Nj() const { return get_size_t("Nj"); }
    
    double i_start() const { return get_double("longitudeOfFirstGridPointInDegrees"); }
    double i_step() const {  return get_double("iDirectionIncrementInDegrees"); }
    double i_stop() const {  return get_double("longitudeOfLastGridPointInDegrees"); }
    
    double j_start() const { return get_double("latitudeOfFirstGridPointInDegrees"); }
    double j_step() const {  return get_double("jDirectionIncrementInDegrees"); }
    double j_stop() const {  return get_double("latitudeOfLastGridPointInDegrees"); }
    
    struct iterator : std::unique_ptr<codes_iterator, decltype(&codes_grib_iterator_delete)> {
        using t = std::unique_ptr<codes_iterator, decltype(&codes_grib_iterator_delete)>;
        
        explicit iterator(codes_handle* h) : t(codes_grib_iterator_new(h, 0, &err), &codes_grib_iterator_delete) {
            ASSERT(CODES_SUCCESS == err);
            ASSERT(*this);
        }
        
        bool next() { return codes_grib_iterator_next(get(), &lat, &lon, &value) > 0; }
        
        friend std::ostream& operator<<(std::ostream& out, const iterator& it) {
            return (out << "- lat=" << it.lat << " lon=" << it.lon << " value=" << it.value);
        }
        
        int err      = 0;
        double lat   = 0;
        double lon   = 0;
        double value = 0;
    };
#endif


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
    const auto* s = "";

    out << '{';
    for (const auto& v : vec) {
        out << s << v;
        s = ", ";
    }

    return out << '}';
}


int main(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        auto* in = std::fopen(argv[i], "rb");
        ASSERT(in != nullptr && "unable to open file");

        int err = 0;
        for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            eckit::grid::grib::GribConfiguration grib(h);

#if 0
            int n = 0;
            for (grib::iterator it(h); it.next(); ++n) {
                std::cout << "- " << n << " " << it << "\n";
            }
            std::cout.flush();
#endif

            std::string type;
            ASSERT(grib.get("gridType", type));
            std::cout << "type: '" << type << "'" << std::endl;

            if (grib.has("pl")) {
                //                std::cout << "pl: '" << grib.get<eckit::grid::pl_type>("pl") << "'" << std::endl;
            }

            if (type == "regular_ll") {
                //  struct regular_ll_args {  // also, rotated_ll
                //      size_t n_i;
                //      double i_start;
                //      double i_stop;
                //      double i_step;
                //      size_t n_j;
                //      double j_start;
                //      double j_stop;
                //      double j_step;
                //  };``
                //
                //  struct regular_gg_args {  // also, rotated_gg
                //      size_t n;
                //      size_t n_i;
                //      double i_start;
                //      double i_stop;
                //      double j_start;
                //      double j_stop;
                //  };
                //
                //  struct reduced_gg_args {  // also, reduced_rotated_gg
                //      size_t n;
                //      std::vector<long> n_i;
                //      double i_start;
                //      double i_stop;
                //      double i_step;
                //      double j_start;
                //      double j_stop;
                //      double j_step;
                //  };
                //
                //  struct reduced_ll_args {
                //      std::vector<long> n_i;
                //      double i_start;
                //      double i_stop;
                //      double i_step;
                //      double j_start;
                //      double j_stop;
                //      double j_step;
            };
        }

        std::fclose(in);
    }

    return 0;
}

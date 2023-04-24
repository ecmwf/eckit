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

#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "grit/exception.h"
#include "grit/figure/Sphere.h"
#include "grit/figure/Spheroid.h"
#include "grit/grit.h"


#if 0
static const std::map<std::string, &type_creator> __types{
    {"regular_ll", nullptr}
};
#endif


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


struct grib_type : std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> {
    using t = std::unique_ptr<codes_handle, decltype(&codes_handle_delete)>;

    bool get_bool(const std::string& key) const { return get_long(key) != 0L; }

    double get_double(const std::string& key) const {
        if (auto value = cache_double.find(key); value != cache_double.end()) {
            return value->second;
        }

        double value = 0;
        ASSERT(CODES_SUCCESS == codes_get_double(get(), key.c_str(), &value));

        cache_double[key] = value;
        return value;
    }

    long get_long(const std::string& key) const {
        if (auto value = cache_long.find(key); value != cache_long.end()) {
            return value->second;
        }

        long value = 0;
        ASSERT(CODES_SUCCESS == codes_get_long(get(), key.c_str(), &value));

        cache_long[key] = value;
        return value;
    }

    size_t get_size_t(const std::string& key) const {
        auto value = get_long(key);
        ASSERT(value >= 0);
        return static_cast<size_t>(value);
    }

    std::string get_string(const std::string& key) const {
        char mesg[1024];
        auto length = sizeof(mesg);
        ASSERT(CODES_SUCCESS == codes_get_string(get(), key.c_str(), mesg, &length));
        const std::string value(mesg);

        cache_string[key] = value;
        return value;
    }

    mutable std::map<std::string, double> cache_double;
    mutable std::map<std::string, long> cache_long;
    mutable std::map<std::string, std::string> cache_string;

    explicit grib_type(codes_handle* h) : t(h, &codes_handle_delete) { ASSERT(*this); }

    std::string type() const { return get_string("gridType"); }

    size_t Ni() const { return get_size_t("Ni"); }
    size_t Nj() const { return get_size_t("Nj"); }

    double i_start() const { return get_double("longitudeOfFirstGridPointInDegrees"); }
    double i_step() const { return get_double("iDirectionIncrementInDegrees"); }
    double i_stop() const { return get_double("longitudeOfLastGridPointInDegrees"); }

    double j_start() const { return get_double("latitudeOfFirstGridPointInDegrees"); }
    double j_step() const { return get_double("jDirectionIncrementInDegrees"); }
    double j_stop() const { return get_double("latitudeOfLastGridPointInDegrees"); }

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
};


void test_grib_iterator(codes_handle* h) {
    ASSERT(h != nullptr);

    // long bitmapPresent = 0;
    // ASSERT(CODES_SUCCESS == codes_get_long(h, "bitmapPresent", &bitmapPresent));
}


struct test;

static const std::map<std::string, test*> __builders{};


namespace GRIB {


grit::Figure* make_figure(long code) {
    // Code table 3.2 – Shape of the reference system

    switch (code) {
        case 0:
            return new grit::figure::Sphere(6367470.);
        case 1:
            // Earth assumed spherical with radius specified (in m) by data producer
            NOTIMP;
        case 2:
            return new grit::figure::Spheroid(6378160., 6356775.);
        case 3:
            // Earth assumed oblate spheroid with major and minor axes specified (in km) by data producer
            NOTIMP;
        case 4:
            return new grit::figure::Spheroid(6378137., 6356752.314);
        case 5:
            return new grit::figure::Spheroid(6378137., 6356752.314140347);
        case 6:
            return new grit::figure::Sphere(6371229.);
        case 7:
            // Earth assumed oblate spheroid with major or minor axes specified (in m) by data producer
            NOTIMP;
        case 8:
            return new grit::figure::Sphere(6371200.);
        case 9:
            return new grit::figure::Spheroid(6377563.396, 6356256.909);
        case 10:
            // Earth model assumed WGS84 with corrected geomagnetic coordinates (latitude and longitude) defined by
            // Gustafsson et al., 1992
            NOTIMP;
        case 11:
            return new grit::figure::Sphere(695990000.);
        default:
            NOTIMP;
    }
}


}  // namespace GRIB


int main(int argc, const char* argv[]) {


    for (int i = 1; i < argc; ++i) {
        auto* in = std::fopen(argv[i], "rb");
        ASSERT(in != nullptr && "unable to open file");

        int err = 0;
        for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            grib_type grib(h);

#if 0
            int n = 0;
            for (grib::iterator it(h); it.next(); ++n) {
                std::cout << "- " << n << " " << it << "\n";
            }
            std::cout.flush();
#endif

            auto type = grib.type();
            std::cout << "type: '" << type << "'" << std::endl;

            if (type == "regular_ll") {
                struct regular_ll_args {  // also, rotated_ll
                    size_t n_i;
                    double i_start;
                    double i_stop;
                    double i_step;
                    size_t n_j;
                    double j_start;
                    double j_stop;
                    double j_step;
                };

                regular_ll_args args{grib.Ni(), grib.i_start(), grib.i_stop(), grib.i_step(),
                                     grib.Nj(), grib.j_start(), grib.j_stop(), grib.j_step()};


                //                struct regular_gg_args {  // also, rotated_gg
                //                    size_t n;
                //                    size_t n_i;
                //                    double i_start;
                //                    double i_stop;
                //                    double j_start;
                //                    double j_stop;
                //                };


                //                struct reduced_gg_args {  // also, reduced_rotated_gg
                //                    size_t n;
                //                    std::vector<long> n_i;
                //                    double i_start;
                //                    double i_stop;
                //                    double i_step;
                //                    double j_start;
                //                    double j_stop;
                //                    double j_step;
                //                };


                //                struct reduced_ll_args {
                //                    std::vector<long> n_i;
                //                    double i_start;
                //                    double i_stop;
                //                    double i_step;
                //                    double j_start;
                //                    double j_stop;
                //                    double j_step;
            };
        }

        std::fclose(in);
    }

    return 0;
}

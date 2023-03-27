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

#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "grit/grit.h"


#if 0
struct regular_ll_args {  // also, rotated_ll
    size_t n_i;
    size_t n_j;
    double i_start;
    double i_stop;
    double i_step;
    double j_start;
    double j_stop;
    double j_step;
};


struct regular_gg_args {  // also, rotated_gg
    size_t n;
    size_t n_i;
    double i_start;
    double i_stop;
    double j_start;
    double j_stop;
};


struct reduced_gg_args {  // also, reduced_rotated_gg
    size_t n;
    std::vector<long> n_i;
    double i_start;
    double i_stop;
    double i_step;
    double j_start;
    double j_stop;
    double j_step;
};


struct reduced_ll_args {
    std::vector<long> n_i;
    double i_start;
    double i_stop;
    double i_step;
    double j_start;
    double j_stop;
    double j_step;
};


template <typename R, typename ...ARGS> using function = R(*)(ARGS...);
using type_creator = function<void*, int>;


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


void test_grib_gridtype(codes_handle* h) {
    assert(h != nullptr);

    char mesg[1024];
    auto length = sizeof(mesg);
    assert(CODES_SUCCESS == codes_get_string(h, "gridType", mesg, &length));

    std::string gridType(mesg);

    std::cout << "gridType='" << gridType << "'" << std::endl;
}


struct grib : std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> {
    std::string get_string(const std::string& key) {
        char mesg[1024];
        auto length = sizeof(mesg);
        assert(CODES_SUCCESS == codes_get_string(get(), key.c_str(), mesg, &length));
        return mesg;
    }


public:
    using t = std::unique_ptr<codes_handle, decltype(&codes_handle_delete)>;

    explicit grib(codes_handle* h) : t(h, &codes_handle_delete) {
        assert(*this);
    }

    std::string gridType() {
        return get_string("gridType");
    }

    struct iterator : std::unique_ptr<codes_iterator, decltype(&codes_grib_iterator_delete)> {
        using t = std::unique_ptr<codes_iterator, decltype(&codes_grib_iterator_delete)>;

        explicit iterator(codes_handle* h) : t(codes_grib_iterator_new(h, 0, &err), &codes_grib_iterator_delete) {
            assert(CODES_SUCCESS == err);
            assert(*this);
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
    assert(h != nullptr);

    // long bitmapPresent = 0;
    // assert(CODES_SUCCESS == codes_get_long(h, "bitmapPresent", &bitmapPresent));
}


int main(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        auto* in = std::fopen(argv[i], "rb");
        assert(in != nullptr && "unable to open file");

        int err = 0;
        for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            grib g(h);

#if 0
            int n = 0;
            for (grib::iterator it(h); it.next(); ++n) {
                std::cout << "- " << n << " " << it << "\n";
            }
            std::cout.flush();
#endif
            std::cout << "gridType: '" << g.gridType() << "'" << std::endl;
        }

        std::fclose(in);
    }

    return 0;
}

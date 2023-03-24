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


void test_grib_iterator(codes_handle* h) {
    assert(h != nullptr);

    int err = 0;

    // long bitmapPresent = 0;
    // assert(CODES_SUCCESS == codes_get_long(h, "bitmapPresent", &bitmapPresent));

    auto* it = codes_grib_iterator_new(h, 0, &err);
    assert(CODES_SUCCESS == err);

    int n = 0;
    for (double lat = 0, lon = 0, value = 0; codes_grib_iterator_next(it, &lat, &lon, &value) > 0; ++n) {
        std::cout << "- " << n << " - lat=" << lat << " lon=" << lon << " value=" << value << "\n";
    }
    std::cout.flush();

    codes_grib_iterator_delete(it);
}


int main(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        auto* in = std::fopen(argv[i], "rb");
        assert(in != nullptr && "unable to open file");

        int err = 0;
        for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            assert(CODES_SUCCESS == err);

            test_grib_gridtype(h);
            // test_grib_iterator(h);

            codes_handle_delete(h);
        }

        std::fclose(in);
    }

    return 0;
}

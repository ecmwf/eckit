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

#include "griditer/griditer.h"


int main(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        auto* in = std::fopen(argv[1], "rb");
        assert(in != nullptr && "unable to open file");

        int err = 0;
        for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
            assert(CODES_SUCCESS == err);

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
            codes_handle_delete(h);
        }

        std::fclose(in);
    }

    return 0;
}

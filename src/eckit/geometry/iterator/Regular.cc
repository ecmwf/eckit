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


#include "eckit/geometry/iterator/Regular.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geometry::iterator {


Regular::Regular(size_t ni, size_t nj, double north, double west, double we, double ns) :
    ni_(ni), nj_(nj), north_(north), west_(west), we_(we), ns_(ns), i_(0), j_(0), lat_(north_), lon_(west_), count_(0), first_(true), p_(PointLonLat{0, 0}) {
    latValue_ = lat_;
    lonValue_ = lon_;
}


bool Regular::operator!=(const Iterator&) {
    NOTIMP;
}


bool Regular::operator++() {
    if (j_ < nj_) {
        if (i_ < ni_) {
            p_ = PointLonLat{lonValue_, latValue_};

            lon_ += we_;

            if (first_) {
                first_ = false;
            }
            else {
                count_++;
            }

            if (++i_ == ni_) {
                j_++;
                i_ = 0;
                lat_ -= ns_;
                lon_      = west_;
                latValue_ = lat_;
            }

            lonValue_ = lon_;

            return true;
        }
    }
    return false;
}


bool Regular::operator--() {
    NOTIMP;
}


Regular::operator bool() {
    NOTIMP;
}


Point& Regular::operator*() {
    NOTIMP;
}


size_t Regular::size() const {
    NOTIMP;
}

size_t Regular::index() const {
    NOTIMP;
}


}  // namespace eckit::geometry::iterator

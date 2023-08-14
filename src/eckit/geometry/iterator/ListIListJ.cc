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


#include "eckit/geometry/iterator/ListIListJ.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geometry::iterator {


ListIListJ::ListIListJ(const Grid& grid) :
    Iterator(grid),
    ni_(0),     // size_t ni
    nj_(0),     // size_t nj
    north_(0),  // double north
    west_(0),   // double west
    we_(0),     // double we
    ns_(0),     // double ns
    i_(0),
    j_(0),
    lat_(north_),
    lon_(west_),
    count_(0),
    first_(true),
    p_(PointLonLat{0, 0}) {
    latValue_ = lat_;
    lonValue_ = lon_;
}


bool ListIListJ::operator==(const Iterator&) {
    NOTIMP;
}


bool ListIListJ::operator++() {
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


bool ListIListJ::operator--() {
    NOTIMP;
}


ListIListJ::operator bool() {
    NOTIMP;
}


const Point& ListIListJ::operator*() {
    NOTIMP;
}


size_t ListIListJ::size() const {
    NOTIMP;
}


size_t ListIListJ::index() const {
    NOTIMP;
}


}  // namespace eckit::geometry::iterator

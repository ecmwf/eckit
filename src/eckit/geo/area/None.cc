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


#include "eckit/geo/area/None.h"

#include "eckit/spec/Custom.h"


namespace eckit::geo::area {


const std::string& None::type() const {
    static const std::string type{"none"};
    return type;
}


void None::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
}


}  // namespace eckit::geo::area

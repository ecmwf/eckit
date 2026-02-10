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


#include "eckit/geo/cache/Record.h"

#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::cache {


std::string record_uid_from_spec(const spec::Spec& s) {
    return s.get_string("uid", "");
}


std::string record_url_from_spec(const spec::Spec& s) {
    return LibEcKitGeo::url(s.get_string("url"));
}


}  // namespace eckit::geo::cache

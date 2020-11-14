/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosAttributes.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {


void RadosAttributes::print(std::ostream& s) const {
    s << "RadosAttributes[";

    const char* sep = "";
    for (auto j = attrs_.begin(); j != attrs_.end(); ++j) {
        s << sep;
        s << (*j).first << "=" << (*j).second;
        sep = ",";
    }

    s << "]";
}


}  // namespace eckit

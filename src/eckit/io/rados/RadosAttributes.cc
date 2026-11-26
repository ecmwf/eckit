// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

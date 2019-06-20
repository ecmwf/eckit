/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "Offset.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void sort(OffsetList& offset, LengthList& length) {
    ASSERT(offset.size() == length.size());
    size_t i = 0;

    typedef std::map<Offset, Length> OffsetLengthMap;

    OffsetLengthMap ol;
    for (i = 0; i < offset.size(); i++)
        ol[offset[i]] = length[i];

    i = 0;
    for (OffsetLengthMap::iterator j = ol.begin(); j != ol.end(); ++j, ++i) {
        offset[i] = (*j).first;
        length[i] = (*j).second;
    }
}

bool compress(OffsetList& offset, LengthList& length) {
    ASSERT(offset.size() == length.size());

    size_t j = 0;
    for (size_t i = 1; i < offset.size(); i++)
        if ((offset[j] + length[j]) == offset[i])
            length[j] += length[i];
        else {
            ASSERT(++j < offset.size());
            offset[j] = offset[i];
            length[j] = length[i];
        }

    long save = offset.size() - j - 1;
    if (save > 0) {
        offset.erase(offset.begin() + j + 1, offset.end());
        length.erase(length.begin() + j + 1, length.end());
    }

    return save > 0;
}

void accumulate(const LengthList& length, OffsetList& offset, const Offset& from) {
    offset.clear();
    offset.reserve(length.size());

    Offset o(from);
    for (size_t i = 0; i < length.size(); i++) {
        offset.push_back(o);
        o += length[i];
    }
}

std::ostream& operator<<(std::ostream& s, const Offset& x) {
    return s << x.value_;
}

Stream& operator<<(Stream& s, const Offset& x) {
    return s << x.value_;
}

Stream& operator>>(Stream& s, Offset& x) {
    s >> x.value_;
    return s;
}

void Offset::load(DumpLoad& a) {
    a.load(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

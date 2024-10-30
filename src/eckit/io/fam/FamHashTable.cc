/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamHashTable.h"

#include "detail/FamHashNode.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamObjectName.h"
#include "eckit/io/fam/FamRegionName.h"

// #include "detail/FamSessionDetail.h"
// #include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamHashTable::FamHashTable(const FamRegionName& regionName, const std::string& tableName):
    region_ {regionName.lookup()},
    begin_ {initSentinel(tableName + "-hash-begin", sizeof(FamDescriptor))},
    count_ {initSentinel(tableName + "-hash-count", sizeof(size_type))} { }

auto FamHashTable::initSentinel(const std::string& name, const fam::size_t size) const -> FamObject {
    try {
        return region_.allocateObject(size, name);
    } catch (const AlreadyExists&) {
        auto object = region_.lookupObject(name);
        ASSERT(object.size() == size);
        return object;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

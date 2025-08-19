/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   FileSpaceStrategies.h
/// @date   Mar 1998
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_FileSpaceStrategies_h
#define eckit_FileSpaceStrategies_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileSpaceStrategies : private NonCopyable {
public:

    static const PathName& selectFileSystem(const std::vector<PathName>& fileSystems, const std::string& s);

    static const PathName& leastUsed(const std::vector<PathName>& fileSystems);
    static const PathName& leastUsedPercent(const std::vector<PathName>& fileSystems);
    static const PathName& roundRobin(const std::vector<PathName>& fileSystems);
    static const PathName& pureRandom(const std::vector<PathName>& fileSystems);
    static const PathName& weightedRandom(const std::vector<PathName>& fileSystems);
    static const PathName& weightedRandomPercent(const std::vector<PathName>& fileSystems);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   FileSpaceStrategies.h
/// @date   Mar 1998
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_FileSpaceStrategies_h
#define eckit_FileSpaceStrategies_h

#include "eckit/filesystem/PathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Strategies for selecting a filesystem from a set of candidates.
///
/// All strategies filter out filesystems that are unavailable or exceed
/// @c candidateFileSystemMaxUsedPercent (default 99%). When no candidate
/// survives filtering, strategies fall back to leastUsed().
class FileSpaceStrategies {
public:

    FileSpaceStrategies() = delete;

    /// @brief Dispatch to the named strategy.
    /// @param fileSystems candidate filesystem paths
    /// @param strategy strategy name — one of "roundRobin", "weightedRandom",
    ///          "pureRandom", "weightedRandomPercent", "leastUsedPercent",
    ///          "binnedLeastUsed"; any other value selects "leastUsed"
    /// @return selected path
    static const PathName& selectFileSystem(const std::vector<PathName>& fileSystems, const std::string& strategy);

    /// @brief Pick the filesystem with the most absolute free space.
    /// @note Does not apply the max-utilisation filter. Acts as the
    ///       fallback for all other strategies.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& leastUsed(const std::vector<PathName>& fileSystems);

    /// @brief Pick the filesystem with the highest percentage of free space.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& leastUsedPercent(const std::vector<PathName>& fileSystems);

    /// @brief Cycle through candidates in order, seeded by PID.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& roundRobin(const std::vector<PathName>& fileSystems);

    /// @brief Pick uniformly at random among candidates.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& pureRandom(const std::vector<PathName>& fileSystems);

    /// @brief Pick randomly, weighted by absolute available space.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& weightedRandom(const std::vector<PathName>& fileSystems);

    /// @brief Pick randomly, weighted by percentage of available space.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& weightedRandomPercent(const std::vector<PathName>& fileSystems);

    /// @brief Pick randomly among candidates in the top availability bin.
    ///
    /// Divides the available-space range into eckit config value
    /// @c binnedLeastUsedNumBins bins (default 20). Only candidates in the
    /// topmost bin are eligible; one is chosen uniformly at random.
    /// @param fileSystems candidate filesystem paths
    /// @return selected path
    static const PathName& binnedLeastUsed(const std::vector<PathName>& fileSystems);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

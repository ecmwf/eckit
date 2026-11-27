// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RadosName.h
/// @author Metin Çakırcalı
/// @date   November 2026
///
/// Rules shared by the names of pools, namespaces and objects, and their form in a rados: URI:
///   rados:<pool>   rados:<pool>/<namespace>   rados:<pool>/<namespace>/<name>

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"

namespace eckit::rados {

//----------------------------------------------------------------------------------------------------------------------

/// @returns whether a pool, namespace or object can be addressed by this name.
/// A name with '/', '#' or '?', or equal to "." or "..", does not survive in a URI, where it would
/// silently name something else, and one with a NUL byte is cut short by librados.
bool validName(const std::string& name);

/// @param what describes the name in the error, e.g. "pool"
/// @throws UserError unless validName(name)
void checkName(const std::string& what, const std::string& name);

/// @returns the `count` names in the URI
/// @param form the expected form, for the error, e.g. "rados:<pool>/<namespace>"
/// @throws UserError if the URI does not hold exactly `count` names
std::vector<std::string> namesIn(const eckit::URI& uri, size_t count, const std::string& form);

/// @returns the URI of names joined with '/'
/// @throws UserError if `nspace` is the default (empty) namespace, which a URI cannot express
eckit::URI uriOf(const std::string& joined, const std::string& nspace);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::rados

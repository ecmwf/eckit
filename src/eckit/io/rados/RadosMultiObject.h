// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Metin Çakırcalı
/// @date   November 2026

#pragma once

#include <cstddef>
#include <optional>

#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// An object stored as a sequence of parts, each an object of at most a given size, for data
/// which can be larger than what the cluster accepts in one object.
///
/// The first part has the name of the object, and holds the layout in its attributes "length",
/// "parts" and "maxsize". The following parts are named <name>;part-<index>. Every part but the
/// last holds exactly the maximum part size.
class RadosMultiObject {
public:

    struct Layout {
        Length length{0};       ///< total size of the data
        size_t parts{0};        ///< number of parts, at least 1
        Length maxPartSize{0};  ///< size of every part but the last
    };

    explicit RadosMultiObject(const RadosObject& object);

    const RadosObject& object() const { return object_; }

    /// @returns the object holding a part. The first part, of index 0, is the object itself.
    RadosObject part(size_t index) const;

    /// @returns whether the object is a part, other than the first, of some multipart object
    static bool isPart(const RadosObject& object);

    /// @returns nothing if the object does not exist, or was not written as a multipart object
    std::optional<Layout> layout() const;

    /// Records the layout, in one operation. The first part must exist.
    void layout(const Layout& layout) const;

    /// Removes all the parts. An object which was not written as a multipart object is removed as well.
    /// @returns false if the object did not exist
    bool remove() const;

private:  // members

    RadosObject object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

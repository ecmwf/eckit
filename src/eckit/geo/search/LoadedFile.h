// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/search/MappedFile.h"


namespace eckit::geo::search {


/**
 * @brief k-d tree loaded from its cache file (built as required) into other storage, and queried there
 * @details Loading is exclusive, as building (see lock). Until loaded, or if the storage isn't available, the cache
 * file mapping is used.
 */
class LoadedFile : public MappedFile {
public:

    // -- Destructor

    ~LoadedFile() override;

    // -- Methods

    bool loaded() const { return static_cast<bool>(content_); }

    // -- Overridden methods

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    bool ready() override;
    void commit() override;

    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

protected:

    // -- Constructors

    LoadedFile(const std::string& uid, size_t size);

    // -- Methods

    /// Load the cache file content into storage, @return its address, or nullptr (storage not available)
    virtual char* load() = 0;

private:

    // -- Types

    /// Read-only k-d tree, over the loaded content (storage owned by the derived class)
    struct Content;

    // -- Members

    std::unique_ptr<Content> content_;

    // -- Methods

    void load_content();
};


}  // namespace eckit::geo::search

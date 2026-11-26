// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Metin Çakırcalı
/// @date   June 2019

#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosMultiObjWriteHandle : public eckit::DataHandle {

public:  // methods

    /// @param maxPartSize size of the parts; if 0, the largest object the cluster accepts
    /// @param maxPendingBytes for asynchronous writes: amount of data per part not yet waited for, above which a
    ///   write flushes
    /// @param maxOpenParts for asynchronous writes: number of parts written at a time
    explicit RadosMultiObjWriteHandle(const RadosObject&, const Length& maxPartSize = 0,
                                      RadosWriteMode mode    = RadosWriteMode::Sync,
                                      size_t maxPendingBytes = radosDefaultMaxPendingBytes,
                                      size_t maxOpenParts    = radosDefaultMaxOpenParts);

    ~RadosMultiObjWriteHandle() override;

    std::string title() const override;

public:  // methods

    void openForWrite(const Length& length) override;

    long write(const void* buffer, long length) override;
    void close() override;
    void flush() override;

    Offset position() override;

    void print(std::ostream& s) const override;


private:  // methods

    /// Opens a handle on the next part and makes it the current one.
    void openPart();

    /// Closes the handles held, optionally keeping the one of the part being written.
    void closeParts(bool keepCurrent);

private:  // members

    RadosObject object_;

    Length maxPartSize_;
    RadosWriteMode mode_;
    size_t maxPendingBytes_;
    size_t maxOpenParts_;

    size_t written_{0};
    Offset position_{0};
    size_t part_{0};
    bool opened_{false};
    bool partOpen_{false};  ///< the last handle is the one of a part which is still being written

    std::vector<std::unique_ptr<DataHandle>> handles_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

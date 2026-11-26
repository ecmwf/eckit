// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date Jun 2025

#pragma once

#include <ostream>
#include <string>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosPartHandle : public eckit::DataHandle {

public:  // methods

    RadosPartHandle(const eckit::RadosObject& object, const eckit::Offset& offset, const eckit::Length& length);

    ~RadosPartHandle() override;

    void print(std::ostream& out) const override;

    eckit::Length openForRead() override;

    long read(void* buffer, long length) override;
    void close() override;
    void flush() override;

    eckit::Length size() override;
    eckit::Length estimate() override;
    eckit::Offset position() override;
    eckit::Offset seek(const eckit::Offset& offset) override;
    bool canSeek() const override;

    std::string title() const override;

private:  // members

    eckit::RadosObject object_;
    bool open_;
    eckit::Offset offset_;  ///< absolute start offset of the part within the object (immutable)
    eckit::Length len_;     ///< length of the part
    eckit::Length pos_;     ///< current read position relative to the start of the part
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

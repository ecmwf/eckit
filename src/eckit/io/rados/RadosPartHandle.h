/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
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

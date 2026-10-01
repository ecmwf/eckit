// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamHandle.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamObjectName.h"

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

class FamHandle : public DataHandle {
public:  // methods

    enum class Mode : std::uint8_t {
        CLOSED,
        READ,
        WRITE
    };

    FamHandle(const FamObjectName& name, const Offset& position, const Length& length, bool overwrite);

    FamHandle(const FamObjectName& name, bool overwrite = false);

    Length openForRead() override;

    void openForWrite(const Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    Offset seek(const Offset& offset) override;

    bool canSeek() const override { return true; }

    Offset position() override { return pos_; }

    Length estimate() override { return len_; }

    Length size() override;

private:  // methods

    void open(Mode mode);

    void print(std::ostream& out) const override;

private:  // members

    const FamObjectName name_;

    const bool overwrite_{false};

    Offset pos_{0};
    Length len_{0};

    Mode mode_{Mode::CLOSED};

    std::optional<FamObject> object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Metin Çakırcalı
/// @date   June 2019

#pragma once

#include <cstddef>
#include <ostream>
#include <string>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosMultiObjReadHandle : public eckit::DataHandle {

public:  // methods

    explicit RadosMultiObjReadHandle(const RadosObject& object);

    ~RadosMultiObjReadHandle() override;

    std::string title() const override;

public:  // methods

    Length openForRead() override;
    void openForWrite(const Length& length) override;
    void openForAppend(const Length& length) override;

    long read(void* buffer, long length) override;
    long write(const void* buffer, long length) override;
    void close() override;
    void flush() override;
    void rewind() override;
    Offset seek(const Offset& offset) override;
    bool canSeek() const override { return true; };

    Offset position() override;
    Length estimate() override;
    Length size() override;

    void print(std::ostream& s) const override;


private:  // methods

    /// Reads the attributes describing the parts, unless already read since the handle was last opened or closed.
    void load();

private:  // members

    RadosObject object_;

    bool loaded_{false};
    bool opened_{false};

    Length length_{0};
    size_t parts_{0};
    Length maxPartSize_{0};

    Offset position_{0};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

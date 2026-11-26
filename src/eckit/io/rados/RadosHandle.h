// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Nicolau Manubens
/// @date   June 2019

#pragma once

#include <cstdint>
#include <ostream>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosHandle : public DataHandle {

public:  // methods

    explicit RadosHandle(const RadosObject& object);

    ~RadosHandle() override;

    Length openForRead() override;
    void openForWrite(const Length& length) override;

    long read(void* buffer, long length) override;
    long write(const void* buffer, long length) override;
    void close() override;
    void flush() override;
    Offset seek(const Offset& offset) override;
    bool canSeek() const override { return true; };

    Offset position() override;
    Length estimate() override { return size(); }
    Length size() override;

    void print(std::ostream& s) const override;

protected:  // members

    RadosObject object_;

    uint64_t offset_;
    bool opened_;
    bool write_;
    bool first_write_;

    void open();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

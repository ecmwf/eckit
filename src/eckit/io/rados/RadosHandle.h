// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date   June 2019

#pragma once

#include <cstdint>
#include <ostream>
#include <string>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

class RadosWriteOp;

//----------------------------------------------------------------------------------------------------------------------

/// Reads or writes one object. The object cannot be larger than what the cluster accepts in one
/// object: see RadosMultiObjWriteHandle for larger data.
///
/// Opening for writing replaces the object: whatever it held is gone once something has been
/// written or the handle has been closed.
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
    void rewind() override;
    Offset seek(const Offset& offset) override;
    bool canSeek() const override { return true; }

    Offset position() override;
    Length estimate() override { return size(); }
    Length size() override;

    void print(std::ostream& s) const override;
    std::string title() const override;

protected:  // methods

    /// Carries out a write operation of `length` bytes on the object
    virtual void submit(RadosWriteOp& op, long length);

    bool isOpen() const { return mode_ != Mode::Closed; }

protected:  // members

    RadosObject object_;

private:  // members

    enum class Mode {
        Closed,
        Read,
        Write
    };

    Mode mode_{Mode::Closed};

    uint64_t offset_{0};

    /// The object still has to be emptied. This is done together with the first write, or on
    /// close if nothing was written, rather than in a round trip of its own when opening.
    bool truncate_{false};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    explicit RadosHandle(const RadosObject&);

    ~RadosHandle() override;

    Length openForRead() override;
    void openForWrite(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override { return true; };

    Offset position() override;
    Length estimate() override { return size(); }
    Length size() override;

    void print(std::ostream&) const override;

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

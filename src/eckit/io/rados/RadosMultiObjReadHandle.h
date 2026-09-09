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
/// @date   June 2019

#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MultiHandle;

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

    void print(std::ostream& s) const override;


private:  // members

    RadosObject object_;

    Length length_{0};
    size_t parts_{0};

    std::unique_ptr<MultiHandle> handle_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

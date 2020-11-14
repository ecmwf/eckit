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

#ifndef eckit_io_rados_RadosHandle_h
#define eckit_io_rados_RadosHandle_h

#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {


class RadosHandle : public eckit::DataHandle {

public:  // methods
    RadosHandle(const RadosObject&);
    RadosHandle(const std::string&);

    RadosHandle(Stream&);

    virtual ~RadosHandle() override;

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

    std::string title() const;

public:  // methods
    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;

    virtual Offset position() override;
    virtual Length estimate() override;

    virtual void print(std::ostream&) const override;

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

private:  // members
    RadosObject object_;

    uint64_t offset_;
    bool opened_;
    bool write_;

    void open();

    static ClassSpec classSpec_;
    static Reanimator<RadosHandle> reanimator_;
};

}  // namespace eckit

#endif

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/TeeHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_TeeHandle_h
#define eckit_filesystem_TeeHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TeeHandle : public DataHandle {
public:

    typedef std::vector<DataHandle*> HandleList;

    // -- Contructors

    TeeHandle();
    TeeHandle(DataHandle*, DataHandle*);
    TeeHandle(const HandleList&);
    TeeHandle(Stream&);

    // -- Destructor

    ~TeeHandle();

    // -- Operators

    virtual void operator+=(DataHandle*);

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    void rewind() override;
    void print(std::ostream&) const override;

    bool canSeek() const override { return false; }

    void toRemote(Stream&) const override;
    void toLocal(Stream&) const override;
    DataHandle* toLocal() override;
    void selectMover(MoverTransferSelection&, bool read) const override;

    bool moveable() const override;


    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    HandleList datahandles_;

    // -- Methods

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<TeeHandle> reanimator_;

    mutable std::set<std::string> requiredAttributes_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

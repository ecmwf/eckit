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
/// @author Emanuele Danovaro
/// @author Simon Smart
/// @author Tiago Quintino
/// @date May 96

#ifndef eckit_filesystem_MultiHandle_h
#define eckit_filesystem_MultiHandle_h

#include "eckit/io/DataHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MultiHandle : public DataHandle {
public:
    typedef std::vector<DataHandle*> HandleList;

    // -- Contructors

    MultiHandle();
    MultiHandle(const LengthList&);
    MultiHandle(const HandleList&);
    MultiHandle(const HandleList&, const LengthList&);
    MultiHandle(Stream&);

    // -- Destructor

    virtual ~MultiHandle() override;

    // -- Operators

    virtual void operator+=(DataHandle*);
    virtual void operator+=(const Length&);

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    void restartReadFrom(const Offset& from) override;

    virtual Offset position() override;
    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override;

    virtual bool merge(DataHandle*) override;
    virtual bool compress(bool = false) override;

    virtual Length size() override;
    virtual Length estimate() override;
    virtual void toRemote(Stream&) const override;
    virtual void toLocal(Stream&) const override;
    virtual DataHandle* toLocal() override;
    virtual void cost(std::map<std::string, Length>&, bool) const override;
    virtual std::string title() const override;
    virtual bool moveable() const override;
    virtual const std::set<std::string>& requiredMoverAttributes() const override;
    virtual DataHandle* clone() const override;
    virtual void collectMetrics(const std::string& what) const override;
    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:
    // -- Members

    HandleList datahandles_;
    HandleList::iterator current_;
    LengthList::iterator curlen_;
    LengthList length_;
    Length written_;
    mutable std::set<std::string> requiredAttributes_;
    bool read_;

    // -- Methods

    void openCurrent();
    void open();
    long read1(char*, long);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<MultiHandle> reanimator_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

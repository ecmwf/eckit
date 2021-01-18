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
/// @date   Jun 2020

#ifndef eckit_message_Splitter_h
#define eckit_message_Splitter_h

#include <iosfwd>

namespace eckit {
class DataHandle;
class PeekHandle;

namespace message {

class Message;

//----------------------------------------------------------------------------------------------------------------------


class Splitter {
public:  // methods
    Splitter(eckit::PeekHandle&);

    virtual ~Splitter();

    virtual Message next() = 0;

protected:
    eckit::PeekHandle& handle_;

private:  // methods
    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Splitter& p) {
        p.print(s);
        return s;
    }
};

class SplitterFactory {
public:
    SplitterFactory();

    virtual ~SplitterFactory();

    virtual Splitter* make(eckit::PeekHandle&) const = 0;


    static Splitter* lookup(eckit::PeekHandle&);

protected:
    virtual bool match(eckit::PeekHandle&) const = 0;
};

template <class T>
class SplitterBuilder : public SplitterFactory {
    virtual Splitter* make(eckit::PeekHandle& handle) const override { return new T(handle); }

    virtual bool match(eckit::PeekHandle& handle) const override;
    //     return M(handle);
    // }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace message
}  // namespace eckit

#endif

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
#include <mutex>
#include <vector>

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

//----------------------------------------------------------------------------------------------------------------------

class SplitterBuilderBase {

public:

    SplitterBuilderBase();
    virtual ~SplitterBuilderBase();

    virtual Splitter* make(eckit::PeekHandle&) const = 0;
    virtual bool match(eckit::PeekHandle&) const     = 0;
};

//----------------------------------------------------------------------------------------------------------------------

class SplitterFactory {
public:

    static SplitterFactory& instance();

    Splitter* lookup(eckit::PeekHandle&);

    void enregister(SplitterBuilderBase*);
    void deregister(const SplitterBuilderBase*);

private:

    SplitterFactory()  = default;
    ~SplitterFactory() = default;

    size_t index_ = 0;
    std::vector<SplitterBuilderBase*> decoders_;  // non-owning pointers
    std::mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class SplitterBuilder : public SplitterBuilderBase {
    Splitter* make(eckit::PeekHandle& handle) const override { return new T(handle); }

    bool match(eckit::PeekHandle& handle) const override;
    //     return M(handle);
    // }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace message
}  // namespace eckit

#endif

/*
 * (C) Copyright 2017- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Aug 2017


#ifndef eckit_message_Reader_H
#define eckit_message_Reader_H

#include <iosfwd>
#include <memory>

#include "eckit/io/PeekHandle.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/message/Message.h"


namespace eckit {
class DataHandle;
class PathName;
class Offset;
};  // namespace eckit

namespace eckit::message {

class Message;
class Splitter;

class Reader : public eckit::NonCopyable {
public:

    Reader(eckit::DataHandle*, bool opened = false);
    Reader(eckit::DataHandle&, bool opened = false);

    Reader(const eckit::PathName&);

    ~Reader();

    Message next();
    eckit::Offset position();

private:

    std::unique_ptr<Splitter> splitter_;
    eckit::PeekHandle handle_;

    void init();
    void print(std::ostream&) const;  // Change to virtual if base class

    friend std::ostream& operator<<(std::ostream& s, const Reader& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit::message

#endif

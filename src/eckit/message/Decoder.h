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

#ifndef eckit_message_Decoder_h
#define eckit_message_Decoder_h

#include <iosfwd>


namespace eckit {
namespace message {

class Message;
class MetadataGatherer;
//----------------------------------------------------------------------------------------------------------------------


class Decoder {
public:  // methods
    Decoder();

    virtual ~Decoder();

    virtual void getMetadata(const Message& msg, MetadataGatherer&) const = 0;

    static Decoder& lookup(const Message&);

private:  // methods
    virtual bool match(const Message&) const = 0;
    virtual void print(std::ostream&) const  = 0;

    friend std::ostream& operator<<(std::ostream& s, const Decoder& p) {
        p.print(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace message
}  // namespace eckit

#endif

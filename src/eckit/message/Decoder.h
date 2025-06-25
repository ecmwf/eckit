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

#include "eckit/io/Buffer.h"
#include "eckit/utils/EnumBitmask.h"
#include "eckit/utils/Optional.h"

namespace eckit::message {

class Message;
class MetadataGatherer;

//----------------------------------------------------------------------------------------------------------------------

enum class ValueRepresentation : unsigned {
    Native = 0,
    String = 1,
};


struct GetMetadataOptions {
    ValueRepresentation valueRepresentation{ValueRepresentation::String};
    eckit::Optional<std::string>
        nameSpace{};  // Possible namespaces:
                      //  ls, statistics, parameter, time, geography, vertical, mars
                      //  (https://confluence.ecmwf.int/display/UDOC/What+are+namespaces+-+ecCodes+GRIB+FAQ)
                      // Default: read gribToRequestNamespace from config, if not given use "mars".
                      // To specify all namespaces, use ""
};

//----------------------------------------------------------------------------------------------------------------------


class MessageDecoder {
public:  // methods

    MessageDecoder();

    virtual ~MessageDecoder();

    virtual void getMetadata(const Message& msg, MetadataGatherer& gatherer,
                             const GetMetadataOptions& options = GetMetadataOptions{}) const = 0;

    virtual eckit::Buffer decode(const Message& msg) const = 0;

    static MessageDecoder& lookup(const Message&);

private:  // methods

    virtual bool match(const Message&) const = 0;
    virtual void print(std::ostream&) const  = 0;

    friend std::ostream& operator<<(std::ostream& s, const MessageDecoder& p) {
        p.print(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::message

#endif

/*
 * (C) Copyright 2020 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "Exceptions.h"

#include "eckit/codec/detail/demangle.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

NotEncodable::NotEncodable(const std::string& type_name) :
    Exception{[&type_name] {
        std::stringstream message;
        message << "eckit::codec::NotEncodable: Cannot encode values of type " << type_name << ".";
        message << "\n     Implement the functions"
                   "\n"
                   "\n         void encode_data(const "
                << type_name
                << "&, eckit::codec::Data& );"
                   "\n         size_t encode_metadata(const "
                << type_name
                << "&, eckit::codec::Metadata& );"
                   "\n"
                   "\n     or alternatively a conversion function to eckit::codec::types::array::ArrayReference"
                   "\n"
                   "\n         void interprete(const "
                << type_name
                << "&, eckit::codec::types::array::ArrayReference& )"
                   "\n"
                   "\n     Rules of argument-dependent-lookup apply."
                   "\n     --> Functions need to be declared in namespace of any of the arguments.";
        return message.str();
    }()} {}

//---------------------------------------------------------------------------------------------------------------------

NotDecodable::NotDecodable(const std::string& type_name) :
    Exception{[&type_name] {
        std::stringstream message;
        message << "eckit::codec::NotDecodable: Cannot decode values of type " << type_name << ".";
        message << "\n     Implement the functions"
                   "\n"
                   "\n         void decode( const atlas::io::Metadata&, const atlas::io::Data&, "
                << type_name
                << "& );"
                   "\n"
                   "\n     Rules of argument-dependent-lookup apply."
                   "\n     --> Functions need to be declared in namespace of any of the arguments.";
        return message.str();
    }()} {}

//---------------------------------------------------------------------------------------------------------------------

Exception::~Exception()           = default;
NotEncodable::~NotEncodable()     = default;
NotDecodable::~NotDecodable()     = default;
InvalidRecord::~InvalidRecord()   = default;
DataCorruption::~DataCorruption() = default;
WriteError::~WriteError()         = default;

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec

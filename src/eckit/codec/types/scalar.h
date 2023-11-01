/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/codec/Data.h"
#include "eckit/codec/Metadata.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

size_t encode_metadata(const int&, Metadata&);
size_t encode_metadata(const long&, Metadata&);
size_t encode_metadata(const long long&, Metadata&);
size_t encode_metadata(const unsigned long&, Metadata&);
size_t encode_metadata(const unsigned long long&, Metadata&);
size_t encode_metadata(const float&, Metadata&);
size_t encode_metadata(const double&, Metadata&);

//---------------------------------------------------------------------------------------------------------------------

void encode_data(const int&, Data&);
void encode_data(const long&, Data&);
void encode_data(const long long&, Data&);
void encode_data(const unsigned long&, Data&);
void encode_data(const unsigned long long&, Data&);
void encode_data(const float&, Data&);
void encode_data(const double&, Data&);

//---------------------------------------------------------------------------------------------------------------------

void decode(const Metadata&, const Data&, int&);
void decode(const Metadata&, const Data&, long&);
void decode(const Metadata&, const Data&, long long&);
void decode(const Metadata&, const Data&, unsigned long&);
void decode(const Metadata&, const Data&, unsigned long long&);
void decode(const Metadata&, const Data&, float&);
void decode(const Metadata&, const Data&, double&);

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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

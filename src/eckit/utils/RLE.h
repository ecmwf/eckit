// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File RLE.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_RLE_h
#define eckit_RLE_h

#include <chrono>
#include <iosfwd>

//-----------------------------------------------------------------------------

namespace eckit {

using EncodingClock = std::chrono::steady_clock;

//-----------------------------------------------------------------------------

class Stream;

template <class InputIterator, class OutputIterator>
long long RLEencode2(InputIterator first, InputIterator last, OutputIterator result, long long maxLoop);

template <class InputIterator, class OutputIterator>
long long RLEencode2(InputIterator first, InputIterator last, OutputIterator result, long long maxLoop,
                     const EncodingClock::duration timeLimit, size_t maxDepth = 1000);

template <class InputIterator, class OutputIterator>
void RLEdecode2(InputIterator first, InputIterator last, OutputIterator result);

template <class InputIterator>
void RLEprint(std::ostream&, InputIterator first, InputIterator last);


template <class InputIterator, class OutputIterator>
bool DIFFencode(InputIterator first, InputIterator last, OutputIterator result);

template <class InputIterator, class OutputIterator>
void DIFFdecode(InputIterator first, InputIterator last, OutputIterator result);


//==========================================================================

template <class InputIterator>
Stream& RLEwrite(Stream&, InputIterator, InputIterator, long long);

template <class OutputIterator, class T>
Stream& RLEread(Stream&, OutputIterator, T*);

template <class InputIterator>
Stream& RLEDIFFwrite(Stream&, InputIterator, InputIterator, long long);

template <class OutputIterator, class T>
Stream& RLEDIFFread(Stream&, OutputIterator, T*);


//-----------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/utils/RLE.cc"

#endif

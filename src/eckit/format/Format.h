/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#pragma once

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <iterator>
#include <type_traits>

/*
 * Format wrappers around libfmt
 *
 * For most cases it is encouraged to use the macro `eckit_format`. It will perform compile time checks.
 *
 * For a very specific timecritical cases `eckit_format_cc` can be used to produce very optimized formatting code.
 * Disadvantag: more binary code
 *
 * For other cases where the format string may dynamicall be configured somethere else, the functions `eckit::format`
 * and `eckit::format_to` can be used - here checks are performed at runtime and may throw.
 */


#define ENABLE_FORMAT(typ) \
    template <>            \
    struct fmt::formatter<typ> : fmt::ostream_formatter {}

/// Format a string with compile time checks.
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
///        Must be known at compiletime
/// @param ... args to be upplied into formatString
#define eckit_format(str, ...) fmt::format(FMT_STRING(str), ##__VA_ARGS__)

/// Format s string with compile time optimizations.
/// Converts formatString into a format string that will be parsed at compile time and converted into efficient
/// formatting code.
/// @note Format string compilation can generate more binary code compared to the default API and is only recommended in
/// places where formatting is a performance bottleneck.
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
/// @param ... args to be upplied into formatString
/// @throws fm::format_error if args cannot be applied to formatString or formatString syntax is invalid.
#define eckit_format_cc(str, ...) fmt::format(FMT_COMPILE(str), ##__VA_ARGS__)


/// Format a string with compile time checks and output to an outputiterator or ostream.
/// @param out OutputIterator or ostream
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
///        Must be known at compiletime
/// @param ... args to be upplied into formatString
#define eckit_format_to(out, str, ...) \
    fmt::format_to(eckit::makeOrForwardOutputiterator(out), FMT_STRING(str), ##__VA_ARGS__)

/// Format s string with compile time optimizations and output to an outputiterator or ostream.
/// Converts formatString into a format string that will be parsed at compile time and converted into efficient
/// formatting code.
/// @note Format string compilation can generate more binary code compared to the default API and is only recommended in
/// places where formatting is a performance bottleneck.
/// @param out OutputIterator or ostream
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
/// @param ... args to be upplied into formatString
/// @throws fm::format_error if args cannot be applied to formatString or formatString syntax is invalid.
#define eckit_format_to_cc(out, str, ...) \
    fmt::format_to(eckit::makeOrForwardOutputiterator(out), FMT_COMPILE(str), ##__VA_ARGS__)

namespace eckit {

template <typename OutputIt, std::enable_if_t<!std::is_base_of_v<std::ostream, std::decay_t<OutputIt>>, bool> = true>
OutputIt&& makeOrForwardOutputiterator(OutputIt&& outputIt) {
    return std::forward<OutputIt>(outputIt);
}

std::ostream_iterator<const char&> makeOrForwardOutputiterator(std::ostream& os);


/// Format a string with compile time checks.
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
///        Must be known at compiletime
/// @param ... args to be upplied into formatString
/// @throws fmt::format_error
template <typename StringType, typename... Args>
std::string str_format(StringType&& formatString, Args&&... args) {
    return fmt::format(fmt::runtime(std::forward<StringType>(formatString)), std::forward<Args>(args)...);
}


/// Format a string with compile time checks to an output iterator
/// @param outputIt output iterator or ostream to write to
/// @param formatString to use, see: <https://fmt.dev/11.1/syntax/> for description of syntax.
///        Must be known at compiletime
/// @param ... args to be upplied into formatString
/// @throws fmt::format_error
template <typename OutputIt, typename StringType, typename... Args>
void str_format_to(OutputIt&& outputIt, StringType&& formatString, Args&&... args) {
    fmt::format_to(makeOrForwardOutputiterator(std::forward<OutputIt>(outputIt)),
                   fmt::runtime(std::forward<StringType>(formatString)), std::forward<Args>(args)...);
}


}  // namespace eckit

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
/// @author Tiago Quintino
/// @date Jul 1996

#ifndef eckit_log_Bytes_h
#define eckit_log_Bytes_h

#include "eckit/utils/Literals.h"

#include <iosfwd>
#include <string>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Timer;

class Bytes {

public:  // methods

    Bytes(double);

    /// Computes a rate
    Bytes(double, Timer&);

    /// Computes a rate
    Bytes(double, double);

    operator std::string() const;

    friend std::ostream& operator<<(std::ostream&, const Bytes&);

    /// Compute number of bytes in 'n' Kibibytes.
    /// @note Result will wraparound if n > 2^52-1
    /// @param n nuber of Kibibytes
    static uint64_t KiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_KiB * n;
    }
    /// Compute number of bytes in 'n' Mebibytes.
    /// @note Result will wraparound if n > 2^42-1
    /// @param n nuber of Mebibytes
    static uint64_t MiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_MiB * n;
    }
    /// Compute number of bytes in 'n' Gibibytes.
    /// @note Result will wraparound if n > 2^32-1
    /// @param n nuber of Gibibytes
    static uint64_t GiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_GiB * n;
    }
    /// Compute number of bytes in 'n' Tebibytes.
    /// @note Result will wraparound if n > 2^22-1
    /// @param n nuber of Tebibytes
    static uint64_t TiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_TiB * n;
    }
    /// Compute number of bytes in 'n' Pebibytes.
    /// @note Result will wraparound if n > 4095
    /// @param n nuber of Pebibytes
    static uint64_t PiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_PiB * n;
    }
    /// Compute number of bytes in 'n' Exbibytes.
    /// @note Result will wraparound if n > 3
    /// @param n nuber of Exbibytes
    static uint64_t EiB(uint64_t n) {
        using namespace eckit::literals;
        return 1_EiB * n;
    }

    /// Handle rate computations avoiding floating point exceptions
    static double rate(double num, double den);

    double value() const;

    std::string shorten() const;

private:  // members

    int sign() const;
    std::pair<double, char> reduceTo1024() const;
    std::pair<double, char> reduceTo100() const;

    double bytes_;
    bool rate_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif

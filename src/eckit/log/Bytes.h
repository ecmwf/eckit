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

    static unsigned long long KiB(unsigned long long n) { return 1024 * n; }
    static unsigned long long MiB(unsigned long long n) { return 1024 * KiB(n); }
    static unsigned long long GiB(unsigned long long n) { return 1024 * MiB(n); }
    static unsigned long long TiB(unsigned long long n) { return 1024 * GiB(n); }
    static unsigned long long PiB(unsigned long long n) { return 1024 * TiB(n); }
    static unsigned long long EiB(unsigned long long n) { return 1024 * PiB(n); }
    static unsigned long long ZiB(unsigned long long n) { return 1024 * EiB(n); }
    static unsigned long long YiB(unsigned long long n) { return 1024 * ZiB(n); }

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

// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_NonCopyable_h
#define eckit_NonCopyable_h


namespace eckit {

/// Inherit from this class to make a NonCopyable class

class NonCopyable {
protected:

    NonCopyable();
    ~NonCopyable();

private:  // No copy allowed

    NonCopyable(const NonCopyable&)            = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}  // namespace eckit

#endif

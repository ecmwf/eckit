// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef SPMetadata_H
#define SPMetadata_H


namespace eckit {

template <class Traits>
struct SPMetadata {

    using Point = typename Traits::Point;

    Point offset_;
    Point scale_;
};


}  // namespace eckit


#endif

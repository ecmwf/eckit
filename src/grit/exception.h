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

#include <cassert>
#include <stdexcept>


namespace grit {


#define ASSERT assert
#define ASSERT_MSG(x, y) assert((x) && (y))
#define NOTIMP throw std::runtime_error("Not implemented")


using std::runtime_error;


}  // namespace grit

/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta - ECMWF March 2015

#ifndef Request_H
#define Request_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ostream>

#include "eckit/parser/Cell.h"
#include "eckit/parser/List.h"

namespace eckit {

typedef Cell* Values;
typedef Cell* Request;

} // namespace eckit

#endif

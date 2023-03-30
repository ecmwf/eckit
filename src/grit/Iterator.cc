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


#include "grit/Iterator.h"

#include "grit/Figure.h"
#include "grit/Scanner.h"
#include "grit/Transformation.h"
#include "grit/exception.h"


namespace grit {


Iterator::Iterator(Scanner* scanner, Figure* figure, Transformation* transformation) :
    scanner_(scanner), figure_(figure), transformation_(transformation) {
    ASSERT(bool(scanner));
    ASSERT(bool(figure));
}


bool Iterator::operator++() {
    return ++(*scanner_);
}


}  // namespace grit

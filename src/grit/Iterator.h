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


#include <cstddef>
#include <memory>
#include <vector>

#include "grit/Figure.h"


namespace grit {
struct Figure;
struct Scanner;
struct Transformation;
}  // namespace grit


namespace grit {


struct Iterator {
    Iterator() {}

    Iterator(const Iterator&) = delete;
    Iterator(Iterator&&)      = delete;

    Iterator& operator=(const Iterator&) = delete;
    Iterator& operator=(Iterator&&)      = delete;

    virtual ~Iterator() = delete;

    bool operator++() {
        pos_++;
        return false;
    }

    size_t pos_;

    std::unique_ptr<Scanner*> scanner_;
    std::vector<Transformation*> transformation_;
    //    Figure figure_;
};


}  // namespace grit

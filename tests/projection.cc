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


#include <iostream>
#include <memory>

#include "grit/Projection.h"
#include "grit/projection/Rotation.h"


int main(int argc, char* argv[]) {
    std::unique_ptr<grit::Projection> proj(new grit::projection::Rotation());

    std::cout << proj->direct(grit::PointLatLon{1, 1}) << std::endl;
}

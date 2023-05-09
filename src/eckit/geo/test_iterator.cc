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

#include "grit/Scanner.h"
#include "grit/exception.h"
#include "grit/iterator/IteratorAggregator.h"
#include "grit/iterator/IteratorComposer.h"


int main(int argc, const char* argv[]) {

    grit::iterator::IteratorComposer i(nullptr);


    struct ScannerTest : public grit::Scanner {
        bool operator++() override { NOTIMP; }
        bool operator++(int) override { NOTIMP; }
        size_t size() const override { NOTIMP; }
    };

    grit::iterator::IteratorAggregator<ScannerTest> j;


    return 0;
}

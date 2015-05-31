/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef Stack_H
#define Stack_H

#include <vector>

#include "eckit/parser/Request.h"
#include "RequestHandler.h"

class Stack {
public:
    Stack();

    size_t size() const;
    Cell* top();
    Cell* pop();
    void push(const Request);

private:
    std::vector<Cell*> stack_;

    std::ostream& print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Stack& o) { return o.print(s); }
};

#endif

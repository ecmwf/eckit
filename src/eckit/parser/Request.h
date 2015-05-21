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

//typedef std::vector<std::string>      Values;
//typedef std::map<std::string, Values> Request;

class Cell {
public:
    Cell(const std::string&, Cell*, Cell*);
    Cell(Cell*); // cloning

    const std::string& name() const;
    Cell* value() const;
    Cell* rest() const;

    Cell* name(const std::string&);
    Cell* value(Cell*);
    Cell* rest(Cell*);

    Cell* append(Cell*);

    std::ostream& dot(std::ostream&, const std::string&) const;

    std::string str() const;

    void showGraph(bool background = false);
    void showGraph(const std::string&, bool background = false);

private:
    std::string name_;
    Cell *value_;
    Cell *rest_;

    std::ostream& print(std::ostream&, size_t depth=0) const;
    std::ostream& printAttributes(std::ostream&, size_t depth=0) const;
    std::ostream& printValues(std::ostream&, size_t depth=0) const;

    std::ostream& printDot(std::ostream&, bool=true) const;

    friend std::ostream& operator<< (std::ostream& s, const Cell *r) { return r->print(s); }
};

typedef Cell* Values;
typedef Cell* Request;

#endif

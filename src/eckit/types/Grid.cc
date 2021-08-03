/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Grid.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Helper which contains all possible grids
/// @note Not thread safe

struct Grids {
public:
    static char lookUp(const Grid& g);
    static const Grid& lookUp(char c);

private:
    typedef std::map<char, Grid, std::less<char> > CharGridTable;
    typedef std::map<Grid, char, std::less<Grid> > GridCharTable;

    static CharGridTable charGridTable_;
    static GridCharTable gridCharTable_;
    static char nextChar_;
};

Grids::CharGridTable Grids::charGridTable_;
Grids::GridCharTable Grids::gridCharTable_;
char Grids::nextChar_ = 'a';

char Grids::lookUp(const Grid& g) {
    GridCharTable::iterator i = gridCharTable_.find(g);
    if (i != gridCharTable_.end())
        return (*i).second;

    gridCharTable_[g]         = nextChar_;
    charGridTable_[nextChar_] = g;

    ASSERT(nextChar_ != 'z');

    return nextChar_++;
}

const Grid& Grids::lookUp(char c) {
    CharGridTable::iterator i = charGridTable_.find(c);
    ASSERT(i != charGridTable_.end());
    return (*i).second;
}

//----------------------------------------------------------------------------------------------------------------------

Grid::Grid(const std::vector<double>& n) {
    switch (n.size()) {
        case 0:
            northSouth_ = eastWest_ = undef;
            break;

        case 1:  // For the moment, only support reduced gaussian
            northSouth_ = n[0];
            eastWest_   = 0;
            break;

        case 2:
            northSouth_ = n[0];
            eastWest_   = n[1];
            break;

        default:
            throw UserError("Invalid grid specified");
    }
}

Grid::Grid(const std::string& s) {
    Tokenizer parse("/");
    std::vector<std::string> result;

    parse(s, result);

    switch (result.size()) {
        case 1:  // For the moment, only support reduced gaussian
            northSouth_ = atof(result[0].c_str());
            eastWest_   = 0;
            break;

        case 2:
            northSouth_ = atof(result[0].c_str());
            eastWest_   = atof(result[1].c_str());
            break;

        default:
            throw UserError("Invalid grid specified");
    }

    Log::userInfo() << "GRID: " << s << " ---- " << *this << std::endl;
}

long Grid::score(const Grid& p) const {
    double x = northSouth() / p.northSouth();
    double y = eastWest() / p.eastWest();
    int s    = 0;

    if (x == long(x))
        s++;
    if (y == long(y))
        s++;

    return s;
}

double Grid::distance(const Grid& p) const {

    double a = northSouth_ - p.northSouth_;
    double b = eastWest_ - p.eastWest_;

    /* return ::sqrt(a*a + b*b); */
    return (a * a + b * b);  // No need for sqrt, to speed up
}


Grid::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

void Grid::print(std::ostream& s) const {
    if (undefined())
        s << "(undefined)";
    else {
        if (northSouth_)
            s << northSouth_;
        if (northSouth_ && eastWest_)
            s << "/";
        if (eastWest_)
            s << eastWest_;
    }
}

char Grid::gridName(const Grid& g) {
    return Grids::lookUp(g);
}

Grid Grid::grid(const char c) {
    return Grids::lookUp(c);
}

bool Grid::operator<(const Grid& other) const {
    double x = (northSouth_ * northSouth_) + (eastWest_ * eastWest_);
    double y = (other.northSouth_ * other.northSouth_) + (other.eastWest_ * other.eastWest_);

    // distance or choice
    return (x != y) ? (x < y) : (northSouth_ < other.northSouth_);
}

const Grid& Grid::bestMatch(const std::vector<Grid>& v) const {

    Log::userInfo() << "Grid::bestMatch " << v.size() << std::endl;
    Log::info() << "Grid::bestMatch " << v.size() << std::endl;
    for (std::vector<Grid>::const_iterator j = v.begin(); j != v.end(); ++j) {
        Log::userInfo() << "Grid::bestMatch " << (*j) << std::endl;
        Log::info() << "Grid::bestMatch " << (*j) << std::endl;
    }

    ASSERT(v.size() > 0);

    // Perfect match
    if (std::find(v.begin(), v.end(), *this) != v.end())
        return *this;

    long smax  = score(v[0]);
    int choice = 0;

    for (size_t j = 1; j < v.size(); ++j) {
        long s = score(v[j]);

        if ((s > smax) || (s == smax && v[choice] > v[j])) {
            smax   = s;
            choice = j;
        }
    }

    return v[choice];
}

void Grid::dump(DumpLoad& a) const {
    a.dump(northSouth_);
    a.dump(eastWest_);
}

void Grid::load(DumpLoad& a) {
    a.load(northSouth_);
    a.load(eastWest_);
}

Grid::InvalidGrid::InvalidGrid(const std::string& s) :
    Exception("Invalid Mars Grid '" + s + "'") {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

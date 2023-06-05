/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StatCollector.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef eckit_StatCollector_h
#define eckit_StatCollector_h

#include <cstddef>
#include <iostream>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/BigNum.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

struct StatCollector {
    StatCollector() {
        statsReset();
        depth_ = 0;
    }


    // -- Methods

    void statsCall() { calls_++; }
    void statsVisitNode() { nodes_++; }
    void statsDepth(size_t d) {
        if (d > depth_) {
            depth_ = d;
        }
    }

    void statsNewCandidateOK() { newCandidateOK_++; }
    void statsNewCandidateMiss() { newCandidateMiss_++; }
    void statsCrossOver() { crossOvers_++; }
    void statsReset() { crossOvers_ = calls_ = newCandidateOK_ = newCandidateMiss_ = nodes_ = 0; }

    void print(std::ostream& s) const {
        s << "Stats calls: " << BigNum(calls_)
          << " avg candidates: " << BigNum(double(newCandidateMiss_ + newCandidateOK_) / double(calls_) + 0.5)
          << ", avg nodes: " << BigNum(double(nodes_) / double(calls_) + 0.5) << ", depth: " << depth_;
    }

    void statsPrint(std::ostream& s, bool fancy) const {
        if (fancy) {
            s << *this << std::endl;
        }
        else {
            s << "   calls: " << BigNum(calls_) << std::endl;
            s << "   miss: " << BigNum(newCandidateMiss_) << std::endl;
            s << "   hit: " << BigNum(newCandidateOK_) << std::endl;
            s << "   nodes: " << BigNum(nodes_) << std::endl;
            s << "   depth: " << BigNum(depth_) << std::endl;
            s << "   crossovers: " << BigNum(crossOvers_) << std::endl;
        }
    }

    // -- Members

    size_t calls_;
    size_t nodes_;
    size_t depth_;

    size_t newCandidateMiss_;
    size_t newCandidateOK_;
    size_t crossOvers_;


    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const StatCollector& p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------
//
}  // namespace eckit

#endif

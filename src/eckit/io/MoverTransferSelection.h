/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MoverTransferSelection.h
// Baudouin Raoult - (c) ECMWF Jun 23


#ifndef eckit_MoverTransferSelection_h
#define eckit_MoverTransferSelection_h

#include <map>
#include <set>
#include <string>


#include "eckit/io/Length.h"

//-----------------------------------------------------------------------------

namespace eckit {

class NodeInfo;

//-----------------------------------------------------------------------------


class MoverTransferSelection {
public:

    MoverTransferSelection();
    ~MoverTransferSelection();

    // -- Methods

    void updateCost(const NodeInfo&, const Length& length);
    void updateCost(const std::string&, const Length& length);

    void requiredMoverAttributes(const std::set<std::string>& attrs);

    void preferredMover(const NodeInfo&);
    void preferredMover(const std::string&);

    NodeInfo selectedMover();

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    MoverTransferSelection(const MoverTransferSelection&);
    MoverTransferSelection& operator=(const MoverTransferSelection&);

    // -- Members
    std::set<std::string> moverAttributes_;
    std::map<std::string, Length> cost_;
    std::string preferredMover_;

    // -- Methods
    void selectedMover(NodeInfo&, bool&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const MoverTransfer& p)
    //	{ p.print(s); return s; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif

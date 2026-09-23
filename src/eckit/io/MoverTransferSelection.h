// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

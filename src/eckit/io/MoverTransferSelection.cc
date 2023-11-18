/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/MoverTransferSelection.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"

#include "eckit/runtime/Metrics.h"



namespace eckit {


MoverTransferSelection::MoverTransferSelection() {}
MoverTransferSelection::~MoverTransferSelection(){}


void MoverTransferSelection::updateCost(const std::string& name, const eckit::Length& length) {
    cost_[name] += length;
}


void MoverTransferSelection::updateCost(const eckit::NodeInfo& node, const eckit::Length& length) {
    updateCost(node.node(), length);
}

void MoverTransferSelection::requiredMoverAttributes(const std::set<std::string>& attrs) {
    moverAttributes_.insert(attrs.begin(), attrs.end());
}

void MoverTransferSelection::selectedMover(NodeInfo& result, bool& metrics) {

    metrics = false;

    if (preferredMover_.length() ) {
        if (ClusterNodes::available("mover", preferredMover_)) {
            NodeInfo preferred = ClusterNodes::lookUp("mover", preferredMover_);
            if (preferred.supportsAttributes(moverAttributes_)) {
                Log::info() << "Using preferred mover " << preferredMover_ << std::endl;
                result = preferred;
                return;
            }
            Log::warning() << "Preferred mover "
                           << preferredMover_
                           << " does not support mover attributes: "
                           << moverAttributes_ << std::endl;
        }
        else {
            Log::warning() << "Preferred mover " << preferredMover_
                           << " is not available" << std::endl;
        }
    }

    std::map<std::string, Length> cost;


    for (const auto& c : cost_) {
        if (ClusterNodes::available("mover", c.first) &&
            ClusterNodes::lookUp("mover", c.first).supportsAttributes(moverAttributes_)) {
                cost.insert(c);
            }
    }

    if (cost.empty()) {
        result = ClusterNodes::any("mover", moverAttributes_);
        return;
    }


    Log::info() << "MoverTransfer::transfer cost:" << std::endl;
    for (std::map<std::string, Length>::iterator j = cost.begin(); j != cost.end(); ++j) {
        Log::info() << "   " << (*j).first << " => " << Bytes((*j).second) << std::endl;
    }

    std::string which;
    Length best = 0;

    for (const auto& c : cost) {
        if(c.second >= best) {
            best = c.second;
            which = c.first;
        }
    }

    ASSERT(which != "");


    metrics = true;
    result = ClusterNodes::lookUp("mover", which);
}

NodeInfo MoverTransferSelection::selectedMover() {

    NodeInfo result;
    bool metrics = false;
    selectedMover(result, metrics);

    Metrics::set("mover_node", result.node());
    if (metrics) {
        for (auto j = cost_.begin(); j != cost_.end(); ++j) {
            std::string h        = (*j).first;
            unsigned long long l = (*j).second;
            Metrics::set("mover_costs." + h, l);
        }
    }

    return result;
}

void MoverTransferSelection::preferredMover(const NodeInfo& node) {
    preferredMover(node.node());
}

void MoverTransferSelection::preferredMover(const std::string& name) {
    Log::info() << "MoverTransferSelection::preferredMover " << name << std::endl;
    preferredMover_ = name;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

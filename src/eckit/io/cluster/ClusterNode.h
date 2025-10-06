/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_ClusterNode_h
#define eckit_ClusterNode_h

#include "eckit/memory/NonCopyable.h"

#include <set>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class NodeInfo;

class ClusterNode : private eckit::NonCopyable {
public:

    ClusterNode();

    virtual ~ClusterNode();

    void heartbeat();

    virtual const std::set<std::string>& attributes() const;
    virtual int port() const         = 0;
    virtual void initialise(Stream&) = 0;
    virtual void refresh(Stream&)    = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

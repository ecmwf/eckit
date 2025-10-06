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

#ifndef eckit_io_NodeInfo_h
#define eckit_io_NodeInfo_h

#include "eckit/runtime/TaskID.h"
#include "eckit/types/Types.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class NodeInfo {

public:  // methods

    NodeInfo();

    void port(int p) { port_ = p; }
    int port() const { return port_; }

    void active(bool a) { active_ = a; }
    bool active() const { return active_; }

    void host(const std::string& h) { host_ = h; }
    const std::string& host() const { return host_; }

    void name(const std::string& h) { name_ = h; }
    const std::string& name() const { return name_; }

    void user(const std::string& h) { user_ = h; }
    const std::string& user() const { return user_; }

    void node(const std::string& h) { node_ = h; }
    const std::string& node() const { return node_; }

    void attribute(const std::string& a) { attributes_.insert(a); }
    void attributes(const std::set<std::string>& a) { attributes_ = a; }
    const std::set<std::string>& attributes() const { return attributes_; }
    bool supportsAttributes(const std::set<std::string>& attrs) const;

    void id(TaskID p) { id_ = p; }
    TaskID id() const { return id_; }

    void task(long p) { task_ = p; }
    long task() const { return task_; }


    NodeInfo& init();

    static NodeInfo& thisNode();
    static NodeInfo acceptLogin(Stream&);
    static NodeInfo sendLogin(Stream&);

private:  // members

    std::string name_;
    std::string node_;
    std::string user_;
    std::string host_;
    std::set<std::string> attributes_;

    int port_;
    bool active_;
    TaskID id_;
    long task_;

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const NodeInfo& p) {
        p.print(s);
        return s;
    }

    friend void operator<<(Stream&, const NodeInfo&);
    friend void operator>>(Stream&, NodeInfo&);
};

// Used by MappedArray

inline unsigned long version(NodeInfo*) {
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

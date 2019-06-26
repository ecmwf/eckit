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
/// @date   June 2019

#ifndef eckit_io_rados_RadosCluster_h
#define eckit_io_rados_RadosCluster_h

#include <memory>
#include <string>

#include <rados/librados.h>

#include "eckit/io/Length.h"


namespace eckit {



#define RADOS_CALL(a) eckit::rados_call(a, #a, __FILE__, __LINE__, __func__)

class RadosCluster  {
public:

    Length maxObjectSize() const;

    rados_t cluster() const { return cluster_; }

    static const RadosCluster& instance();

private:

    RadosCluster();

    ~RadosCluster();


private:

    rados_t cluster_;

public:
    static void error(int code, const char *msg, const char* file, int line, const char* func);

};


static inline int rados_call(int code, const char *msg, const char* file, int line, const char* func) {
    if (code < 0) {
        RadosCluster::error(code, msg, file,  line, func);
    }
    return code;
}


}  // namespace eckit

#endif

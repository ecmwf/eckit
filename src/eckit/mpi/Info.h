/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Info_h
#define eckit_mpi_Info_h

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

struct Info {

    enum Code {
        INFO_NULL = 0,
        MAX_INFO_CODE,
    };
};

Info::Code info_null();

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif

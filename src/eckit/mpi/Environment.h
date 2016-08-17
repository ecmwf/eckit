/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Environment_h
#define eckit_mpi_Environment_h

#include <string>

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm;

class Environment {

public:  // methods

  static Environment& instance();

  /// @returns the default communicator
  Comm& comm() const;

  /// @returns the communicator registered with associated name
  Comm& comm(const std::string& name) const;

protected:

  Environment();

  virtual ~Environment();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif

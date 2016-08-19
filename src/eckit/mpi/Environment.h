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
#include <vector>

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Comm;

/// @returns the communicator registered with associated name, or default communicator when NULL is passed
Comm& comm(const char* name = 0);

typedef int Operation;

Operation sum();
Operation prod();
Operation max();
Operation min();
Operation maxloc();
Operation minloc();

//----------------------------------------------------------------------------------------------------------------------

/// Buffer handles colleciton of vector pieces into a larger vector

template <typename DATA_TYPE>
struct Buffer
{
  typedef DATA_TYPE value_type;
  typedef typename std::vector<DATA_TYPE>::iterator iterator;
  int                    cnt;
  std::vector<int>       counts;
  std::vector<int>       displs;
  std::vector<DATA_TYPE> buf;

  Buffer(size_t size)
  {
    counts.resize( size );
    displs.resize( size );
  }

  iterator begin() { return buf.begin(); }
  iterator end()   { return buf.end();   }
};

//----------------------------------------------------------------------------------------------------------------------

class Environment {

public:  // methods

  static Environment& instance();

  /// @brief Initialize MPI
  virtual void initialize() = 0;

  /// @brief Finalze MPI
  virtual void finalize() = 0;

  /// @brief is MPI initialized?
  virtual bool initialized() = 0;

  /// @returns the communicator registered with associated name, or default communicator when NULL is passed
  virtual Comm& comm(const char* name = 0) const = 0;

  virtual Operation sum()  const = 0;
  virtual Operation prod() const = 0;
  virtual Operation max()  const = 0;
  virtual Operation min()  const = 0;
  virtual Operation maxloc() const = 0;
  virtual Operation minloc() const = 0;

protected:

  Environment();

  virtual ~Environment();

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif

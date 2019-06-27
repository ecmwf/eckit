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

#ifndef eckit_io_rados_RadosHandle_h
#define eckit_io_rados_RadosHandle_h

#include <memory>

#include <rados/librados.h>

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

class RadosHandle : public eckit::DataHandle {

public:  // methods

  RadosHandle(const std::string&);
  RadosHandle(Stream&);

  virtual ~RadosHandle();

  // -- Class methods

  static const ClassSpec& classSpec() { return classSpec_; }

  std::string title() const;

public:  // methods

  virtual Length openForRead();
  virtual void openForWrite(const Length&);
  virtual void openForAppend(const Length&);

  virtual long read(void*, long);
  virtual long write(const void*, long);
  virtual void close();
  virtual void flush();
  virtual void rewind();

  virtual Offset position();

  virtual void print(std::ostream&) const;

  // From Streamable

  virtual void encode(Stream&) const;
  virtual const ReanimatorBase& reanimator() const { return reanimator_; }

private:  // members

  std::string name_;

  static ClassSpec classSpec_;
  static Reanimator<RadosHandle> reanimator_;

};

}  // namespace eckit

#endif

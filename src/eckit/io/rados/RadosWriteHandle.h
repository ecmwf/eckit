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

#ifndef eckit_io_rados_RadosWriteHandle_h
#define eckit_io_rados_RadosWriteHandle_h

#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {


class RadosWriteHandle : public eckit::DataHandle {

public:  // methods

  RadosWriteHandle(const RadosObject&, const Length& maxObjectSize = 0);
  RadosWriteHandle(const std::string&, const Length& maxObjectSize = 0);
  RadosWriteHandle(Stream&);

  virtual ~RadosWriteHandle();

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

  RadosObject object_;

  Length maxObjectSize_;
  size_t written_;
  Offset position_;
  size_t part_;

  std::unique_ptr<DataHandle> handle_;


  static ClassSpec classSpec_;
  static Reanimator<RadosWriteHandle> reanimator_;

};

}  // namespace eckit

#endif

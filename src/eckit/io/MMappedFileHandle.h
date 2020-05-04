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
/// @date   May 2020

#ifndef eckit_io_MMappedFileHandle_h
#define eckit_io_MMappedFileHandle_h

#include <memory>
#include "eckit/io/DataHandle.h"


namespace eckit {

class MMappedFileHandle : public DataHandle {

public:
  MMappedFileHandle(const std::string&);
  MMappedFileHandle(Stream&);

  virtual ~MMappedFileHandle();

  const std::string& path() const { return path_; }

  // -- Overridden methods

  // From DataHandle

  virtual Length openForRead();
  virtual void openForWrite(const Length&);
  virtual void openForAppend(const Length&);

  virtual long read(void*, long);
  virtual long write(const void*, long);
  virtual void close();
  virtual void flush();
  virtual void rewind();
  virtual void print(std::ostream&) const;
  virtual Length size();
  virtual Length estimate();
  virtual Offset position();
  virtual bool isEmpty() const;
  virtual void restartReadFrom(const Offset& from);
  virtual void restartWriteFrom(const Offset& from);

  virtual std::string title() const;

  virtual Offset seek(const Offset&);
  virtual void skip(const Length&);

  virtual DataHandle* clone() const;
  virtual void hash(MD5& md5) const;

  // From Streamable

  virtual void encode(Stream&) const;
  virtual const ReanimatorBase& reanimator() const { return reanimator_; }

  // -- Class methods

  static const ClassSpec& classSpec() { return classSpec_; }

private:  // members
  std::string path_;


  std::unique_ptr<DataHandle> handle_;
  void *mmap_;
  int fd_;
  off_t length_;

private:  // methods
  void open(const char*);

  static ClassSpec classSpec_;
  static Reanimator<MMappedFileHandle> reanimator_;
};

}  // namespace eckit

#endif

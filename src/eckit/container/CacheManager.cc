/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "CacheManager.h"

#include <sys/stat.h>

#include <string>

#include "eckit/config/Resource.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class AutoUmask {
  mode_t umask_;

 public:
  explicit AutoUmask(mode_t u = 0) : umask_(::umask(u)) {}
  ~AutoUmask() { ::umask(umask_); }
};

CacheManager::CacheManager(const std::string& name, const std::string& version) : name_(name), version_(version) {}

bool CacheManager::get(const key_t& k, PathName& v) const {
  PathName p = entry(k);

  if (p.exists()) {
    v = p;
    return true;
  }
  return false;
}

PathName CacheManager::stage(const key_t& k) const {

  PathName p = entry(k);
  AutoUmask umask(0);
  // FIXME: mask does not seem to affect first level directory
  p.dirName().mkdir(0777);  // ensure directory exists
  Log::info() << "CacheManager staging file " << p << std::endl;
  // unique file name avoids race conditions on the file from multiple processes
  return PathName::unique(p);
}

bool CacheManager::commit(const key_t& k, const PathName& tmpfile) {
  PathName file = entry(k);
  Log::info() << "CacheManager committing file " << file << std::endl;
  try {
    SYSCALL(::chmod(tmpfile.asString().c_str(), 0444));
    PathName::rename(tmpfile, file);
  } catch (FailedSystemCall& e) {  // ignore failed system call -- another process nay have created the file meanwhile
    Log::debug() << "Failed rename of cache file -- " << e.what() << std::endl;
    return false;
  }
  return true;
}

void CacheManager::print(std::ostream& s) const { NOTIMP; }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

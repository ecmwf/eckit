/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Simon Smart
/// @date   June 2018

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "eckit/exceptions/Exceptions.h"

namespace eckit {

template <typename ELEM>
class Queue {
public:

  Queue(size_t max) : max_(max) {
    ASSERT(max > 0);
  }

  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  void resize(size_t size) {
    ASSERT(size > 0);
    std::unique_lock<std::mutex> locker(mutex_);
    while (queue_.empty()) {
      cv_.wait(locker);
    }
    max_ = size;
    locker.unlock();
    cv_.notify_one();
  }

  ELEM pop() {
    std::unique_lock<std::mutex> locker(mutex_);
    while (queue_.empty()) {
      cv_.wait(locker);
    }
    auto e = queue_.front();
    queue_.pop();
    locker.unlock();
    cv_.notify_one();
    return e;
  }

  void push(const ELEM& e)
  {
    std::unique_lock<std::mutex> locker(mutex_);
    while (queue_.size() >= max_) {
      cv_.wait(locker);
    }
    queue_.push(e);
    locker.unlock();
    cv_.notify_one();
  }


private:
  std::queue<ELEM> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t max_;
};

} // namespace eckit

#endif

#ifndef message_queue_h
#define message_queue_h

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>
#include <chrono>

template<typename T>
class message_queue
{
    size_t max_;

    std::queue<T> queue_;
    mutable std::mutex m_;
    std::condition_variable data_cond_;

    message_queue& operator=(const message_queue&) = delete;
    message_queue(const message_queue& other) = delete;

public:

  message_queue( size_t max = 0 ) : max_(max){}

  void wait_and_push( T item )
  {
      if( max_ )
      {
        std::unique_lock<std::mutex> lock(m_);
        while( queue_.size() >= max_ )
        {
          data_cond_.wait(lock);
        }
        queue_.push(item);
        data_cond_.notify_one();
      }
      else
          push(item);
  }

  void push(T item)
  {
    std::lock_guard<std::mutex> lock(m_);
    queue_.push(item);
    data_cond_.notify_one();
  }

  bool try_and_pop(T& item)
  {
    std::lock_guard<std::mutex> lock(m_);
    if(queue_.empty()){
      return false;
    }
    item = queue_.front();
    queue_.pop();
    data_cond_.notify_one();
    return true;
  }

  void wait_and_pop(T& item)
  {
    std::unique_lock<std::mutex> lock(m_);
    while(queue_.empty())
    {
      data_cond_.wait(lock);
    }
    item = queue_.front();
    queue_.pop();
    data_cond_.notify_one();
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(m_);
    return queue_.empty();
  }

  unsigned size() const
  {
    std::lock_guard<std::mutex> lock(m_);
    return queue_.size();
  }
};

#endif

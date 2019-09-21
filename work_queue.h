#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <condition_variable> // for std::condition_variable
#include <functional> // for std::bind
#include <future> // for std::packaged_task
#include <mutex> // for std::mutex
#include <queue> // for std::queue
#include <thread> // for std::thread

/// std::thread-backed work queue using std::future for return values
class WorkQueue
{
public:
  /// create an idle work queue
  WorkQueue()
    : m_shouldStop(false)
    , m_thread(&WorkQueue::Worker, this)
  {}

  /// @brief  shut down the work queue
  /// @note  waits for the currently processing work item to be finished,
  ///        but cancels all further pending ones
  ~WorkQueue()
  {
    // notify the worker
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_shouldStop = true;
    }
    m_cv.notify_one();

    // wait for worker to finish
    if(m_thread.joinable()) {
      m_thread.join();
    }
  }

  /// @brief  assign a work load to the queue
  /// @param  fn  callable in the form of a function, member function or lambda
  /// @param  args  callable arguments;
  ///         cannot handle non-copyable arguments; work around e.g. wrapping in std::shared_ptr
  /// @return  future to wait for work load completition and return value or exception
  /// @note  work queue size is not limited
  template<typename Fn, typename... Args>
  std::future<
    typename std::result_of<Fn(Args...)>::type
  >
  Assign(Fn&& fn, Args&&... args)
  {
    using ReturnType = typename std::result_of<Fn(Args...)>::type;
    using BoundTask = typename std::packaged_task<ReturnType()>;

    // make a type-erased task with all its arguments bound
    auto task = std::make_unique<TypeErased<BoundTask>>(
      std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));

    // grab the future to return
    std::future<ReturnType> future = task->get_future();

    { // move the task to the back of the work queue
      std::unique_lock<std::mutex> lock(m_mutex);
      m_queue.push(std::move(task));
    }

    // notify the worker
    m_cv.notify_one();

    return future;
  }

private:
  struct AbstractTask
  {
    virtual ~AbstractTask() = default;
    virtual void operator()() = 0;
  };

  template<typename ErasedType>
  struct TypeErased
    : public AbstractTask
    , public ErasedType
  {
    template<typename... Args>
    TypeErased(Args&&... args)
      : ErasedType(std::forward<Args>(args)...)
    {}

    virtual ~TypeErased() = default;

    void operator()() override
    {
      (void)ErasedType::operator()();
    }
  };

private:
  inline void Worker()
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    for(;;) {
      // wait for work
      m_cv.wait(lock,
        [this]() -> bool {
          return (!m_queue.empty() || m_shouldStop);
        });

      if(m_shouldStop) {
        break;
      } else {
        // get a task from the front of the work queue
        auto task = std::move(m_queue.front());
        m_queue.pop();

        // execute the task while releasing the lock
        lock.unlock();
        (*task)();
        lock.lock();
      }
    }
  }

private:
  std::mutex m_mutex;
  bool m_shouldStop;  // guarded by m_mutex
  std::thread m_thread;
  std::queue<std::unique_ptr<AbstractTask>> m_queue;  // guarded by m_mutex
  std::condition_variable m_cv;
};

#endif // WORK_QUEUE_H

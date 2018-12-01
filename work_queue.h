#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class WorkQueue
{
public:
  WorkQueue()
    : m_shouldStop(false)
    , m_thread(&WorkQueue::Worker, this)
  {}

  ~WorkQueue()
  {
    // notify the worker
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_shouldStop = true;
    }
    m_cv.notify_one();

    // wait for worker to finish
    if(m_thread.joinable())
      m_thread.join();
  }

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
  inline void Worker()
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    for(;;) {
      // wait for work
      m_cv.wait(lock,
        [this]() -> bool
        {
          return (!m_queue.empty() || m_shouldStop);
        });

      if(m_shouldStop) {
        break;
      } else {
        // get a task from the front of the work queue
        auto task = std::move(m_queue.front());
        m_queue.pop();

        // execute the task
        lock.unlock();
        (*task)();
        lock.lock();
      }
    }
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
  std::mutex m_mutex;
  bool m_shouldStop;  // guarded by m_mutex
  std::thread m_thread;
  std::queue<std::unique_ptr<AbstractTask>> m_queue;  // guarded by m_mutex
  std::condition_variable m_cv;
};

#endif // WORK_QUEUE_H

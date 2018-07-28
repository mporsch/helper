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

    // make a type-erased task with all its arguments bound
    auto task = std::make_unique<Task<ReturnType>>(
      std::packaged_task<ReturnType()>(
        std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...)));
    auto future = task->get_future();

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
    while(!m_shouldStop) {
      // wait for work
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock,
        [this]() -> bool
        {
          return (!m_queue.empty() || m_shouldStop);
        });

      if(!m_queue.empty()) {
        // get a task from the front of the work queue
        auto task = std::move(m_queue.front());
        m_queue.pop();
        lock.unlock();

        // execture the task
        task->exec();
      }
    }
  }

private:
  struct AbstractTask
  {
    virtual void exec() = 0;
  };

  template<typename ReturnType>
  struct Task : public AbstractTask
  {
    Task(std::packaged_task<ReturnType()> &&task)
      : wrapped(std::move(task))
    {}

    virtual ~Task() = default;

    std::future<ReturnType> get_future()
    {
      return wrapped.get_future();
    }

    void exec() override
    {
      wrapped();
    }

    std::packaged_task<ReturnType()> wrapped;
  };

private:
  bool m_shouldStop;
  std::thread m_thread;
  std::queue<std::unique_ptr<AbstractTask>> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_cv;
};

#endif // WORK_QUEUE_H

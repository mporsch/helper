#ifndef FIRE_AND_DONT_FORGET_H
#define FIRE_AND_DONT_FORGET_H

#include <cassert> // for assert
#include <functional> // for std::invoke
#include <mutex> // for std::mutex
#include <thread> // for std::thread
#include <unordered_map> // for std::unordered_map

namespace fire_and_dont_forget_detail {

#if (defined(_MSC_VER) && (_MSC_VER < 1900)) || (__cplusplus < 201703L)

  // std::invoke implementation adapted from
  // https://en.cppreference.com/w/cpp/utility/functional/invoke

  template<class T>
  struct is_reference_wrapper : std::false_type {};
  template<class U>
  struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
  template<class T>
  constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

  template<class T, class Type, class T1, class... Args>
  decltype(auto) INVOKE(Type T::* f, T1&& t1, Args&&... args)
  {
    if constexpr(std::is_member_function_pointer<decltype(f)>::value) {
      if constexpr(std::is_base_of<T, std::decay_t<T1>>::value) {
        return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
      } else if constexpr(is_reference_wrapper_v<std::decay_t<T1>>) {
        return (t1.get().*f)(std::forward<Args>(args)...);
      } else {
        return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
      }
    } else {
      static_assert(std::is_member_object_pointer<decltype(f)>::value,
                    "expected member object pointer");
      static_assert(sizeof...(args) == 0,
                    "expected zero arguments");

      if constexpr(std::is_base_of<T, std::decay_t<T1>>::value) {
        return std::forward<T1>(t1).*f;
      } else if constexpr(is_reference_wrapper_v<std::decay_t<T1>>) {
        return t1.get().*f;
      } else {
        return (*std::forward<T1>(t1)).*f;
      }
    }
  }

  template<class F, class... Args>
  decltype(auto) INVOKE(F&& f, Args&&... args)
  {
    return std::forward<F>(f)(std::forward<Args>(args)...);
  }

  template<class F, class... Args>
  decltype(auto) invoke(F&& f, Args&&... args)
  {
    return INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
  }

#else // (defined(_MSC_VER) && (_MSC_VER < 1900)) || (__cplusplus < 201703L)

  using std::invoke;

#endif // (defined(_MSC_VER) && (_MSC_VER < 1900)) || (__cplusplus < 201703L)

} // namespace fire_and_dont_forget_detail

class FireAndDontForget
{
public:
  ~FireAndDontForget()
  {
    // grab to local variable before waiting to avoid deadlock
    decltype(m_handles) handles;
    {
      std::lock_guard<std::mutex> lock(m_mtx);
      std::swap(handles, m_handles);
    }

    for(auto&& p : handles) {
      assert(p.second.joinable());
      p.second.join();
    }
  }

  template<typename Fn, typename... Args>
  void Dispatch(Fn&& fn, Args&&... args)
  {
    std::lock_guard<std::mutex> lock(m_mtx);

    // std::decay to handle an argument copy
    m_handles.emplace(
          ToPair(
            std::thread(
              &FireAndDontForget::Run<std::decay_t<Fn>, std::decay_t<Args>...>,
              this,
              std::forward<Fn>(fn),
              std::forward<Args>(args)...)));
  }

private:
  using Handles = std::unordered_map<std::thread::id, std::thread>;

private:
  Handles::value_type ToPair(std::thread t)
  {
    auto id = t.get_id();
    return std::make_pair(std::move(id), std::move(t));
  }

  template<typename Fn, typename... Args>
  void Run(Fn fn, Args... args)
  {
    try {
      (void)fire_and_dont_forget_detail::invoke(
            std::forward<Fn>(fn), std::forward<Args>(args)...);
    } catch(const std::exception&) {
      // ignore
    } catch(...) {
      // ignore
    }

    RemoveMe();
  }

  void RemoveMe()
  {
    std::lock_guard<std::mutex> lock(m_mtx);

    const auto it = m_handles.find(std::this_thread::get_id());
    if(it != std::end(m_handles)) {
      it->second.detach();
      m_handles.erase(it);
    } else {
      // cleared by destructor
      assert(m_handles.empty());
    }
  }

private:
  std::mutex m_mtx;
  Handles m_handles;
};

#endif // FIRE_AND_DONT_FORGET_H

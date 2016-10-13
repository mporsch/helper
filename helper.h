#ifndef HELPER_H
#define HELPER_H

template<typename T>
bool IsAnyEqual(T&&) {
  return false;
}

template<typename T, typename... Args>
bool IsAnyEqual(T&& value, T&& first, Args&&... args) {
  return ((value == first) ||
          (IsAnyEqual(std::forward<T>(value), std::forward<Args>(args)...)));
}


#endif // HELPER_H


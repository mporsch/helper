#ifndef HELPER_H
#define HELPER_H

template<typename T>
bool IsAnyEqual(T value) {
  return false;
}

template<typename T, typename... Args>
bool IsAnyEqual(T value, T first, Args... args) {
  return (value == first) || IsAnyEqual(value, args...);
}


#endif // HELPER_H

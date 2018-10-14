#ifndef PRINT_UNMANGLED_H
#define PRINT_UNMANGLED_H

#include <iostream> // for std::cout
#include <mutex> // for std::mutex

struct PrintUnmangled
{
  static std::mutex &mutex();

  template<typename T>
  friend PrintUnmangled &operator<<(PrintUnmangled &os, T const &value);

  using Ioo = std::ostream &(*)(std::ostream &);
  friend PrintUnmangled &operator<<(PrintUnmangled &os, Ioo const &value);

  using Ios = std::ostream &(*)(std::ios &);
  friend PrintUnmangled &operator<<(PrintUnmangled &os, Ios const &value);

  using Iob = std::ostream &(*)(std::ios_base &);
  friend PrintUnmangled &operator<<(PrintUnmangled &os, Iob const &value);
};

inline std::mutex &PrintUnmangled::mutex()
{
  static std::mutex mtx;
  return mtx;
}

template<typename T>
PrintUnmangled &operator<<(PrintUnmangled &os, T const &value)
{
  std::lock_guard<std::mutex> lock(os.mutex());
  std::cout << value;
  return os;
}

inline PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Ioo const &value)
{
  std::lock_guard<std::mutex> lock(os.mutex());
  std::cout << value;
  return os;
}

inline PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Ios const &value)
{
  std::lock_guard<std::mutex> lock(os.mutex());
  std::cout << value;
  return os;
}

inline PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Iob const &value)
{
  std::lock_guard<std::mutex> lock(os.mutex());
  std::cout << value;
  return os;
}

#endif // PRINT_UNMANGLED_H

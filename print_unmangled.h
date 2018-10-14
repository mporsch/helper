#ifndef PRINT_UNMANGLED_H
#define PRINT_UNMANGLED_H

#include <iostream> // for std::cout
#include <mutex> // for std::mutex
#include <sstream> // for std::ostringstream

struct PrintUnmangled
{
  PrintUnmangled() = default;

  ~PrintUnmangled()
  {
    std::lock_guard<std::mutex> lock(mutex());
    std::cout << oss.str();
  }

  template<typename T>
  friend PrintUnmangled &operator<<(PrintUnmangled &pu, T const &value);

  template<typename T>
  friend PrintUnmangled &operator<<(PrintUnmangled &&pu, T const &value);

  using Ioo = std::ostream &(*)(std::ostream &);
  friend PrintUnmangled &operator<<(PrintUnmangled &pu, Ioo const &value);
  friend PrintUnmangled &operator<<(PrintUnmangled &&pu, Ioo const &value);

  using Ios = std::ostream &(*)(std::ios &);
  friend PrintUnmangled &operator<<(PrintUnmangled &pu, Ios const &value);
  friend PrintUnmangled &operator<<(PrintUnmangled &&pu, Ios const &value);

  using Iob = std::ostream &(*)(std::ios_base &);
  friend PrintUnmangled &operator<<(PrintUnmangled &pu, Iob const &value);
  friend PrintUnmangled &operator<<(PrintUnmangled &&pu, Iob const &value);

private:
  static std::mutex &mutex();

private:
  std::ostringstream oss;
};

inline std::mutex &PrintUnmangled::mutex()
{
  static std::mutex mtx;
  return mtx;
}

template<typename T>
PrintUnmangled &operator<<(PrintUnmangled &pu, T const &value)
{
  pu.oss << value;
  return pu;
}

template<typename T>
PrintUnmangled &operator<<(PrintUnmangled &&pu, T const &value)
{
  return (pu << value);
}

inline PrintUnmangled &operator<<(PrintUnmangled &pu, PrintUnmangled::Ioo const &value)
{
  std::lock_guard<std::mutex> lock(pu.mutex());
  std::cout << pu.oss.str() << value;
  pu.oss.str("");
  pu.oss.clear();
  return pu;
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Ioo const &value)
{
  return (pu << value);
}

inline PrintUnmangled &operator<<(PrintUnmangled &pu, PrintUnmangled::Ios const &value)
{
  std::lock_guard<std::mutex> lock(pu.mutex());
  std::cout << pu.oss.str() << value;
  pu.oss.str("");
  pu.oss.clear();
  return pu;
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Ios const &value)
{
  return (pu << value);
}

inline PrintUnmangled &operator<<(PrintUnmangled &pu, PrintUnmangled::Iob const &value)
{
  std::lock_guard<std::mutex> lock(pu.mutex());
  std::cout << pu.oss.str() << value;
  pu.oss.str("");
  pu.oss.clear();
  return pu;
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Iob const &value)
{
  return (pu << value);
}

#endif // PRINT_UNMANGLED_H

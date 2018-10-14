#ifndef PRINT_UNMANGLED_H
#define PRINT_UNMANGLED_H

#include <iostream> // for std::cout
#include <mutex> // for std::mutex
#include <sstream> // for std::ostringstream

struct PrintUnmangled
{
  PrintUnmangled(std::ostream &os = std::cout)
    : out(os)
  {
  }

  ~PrintUnmangled()
  {
    std::lock_guard<std::mutex> lock(mutex());
    out << oss.str();
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
  PrintUnmangled &cleared();

private:
  std::ostream &out;
  std::ostringstream oss;
};

inline std::mutex &PrintUnmangled::mutex()
{
  static std::mutex mtx;
  return mtx;
}

inline PrintUnmangled &PrintUnmangled::cleared()
{
  oss.str("");
  oss.clear();

  return *this;
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
  {
    std::lock_guard<std::mutex> lock(pu.mutex());
    pu.out << pu.oss.str() << value;
  }
  return pu.cleared();
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Ioo const &value)
{
  return (pu << value);
}

inline PrintUnmangled &operator<<(PrintUnmangled &pu, PrintUnmangled::Ios const &value)
{
  {
    std::lock_guard<std::mutex> lock(pu.mutex());
    pu.out << pu.oss.str() << value;
  }
  return pu.cleared();
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Ios const &value)
{
  return (pu << value);
}

inline PrintUnmangled &operator<<(PrintUnmangled &pu, PrintUnmangled::Iob const &value)
{
  {
    std::lock_guard<std::mutex> lock(pu.mutex());
    pu.out << pu.oss.str() << value;
  }
  return pu.cleared();
}

inline PrintUnmangled &operator<<(PrintUnmangled &&pu, PrintUnmangled::Iob const &value)
{
  return (pu << value);
}

#endif // PRINT_UNMANGLED_H

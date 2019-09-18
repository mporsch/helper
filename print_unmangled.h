#ifndef PRINT_UNMANGLED_H
#define PRINT_UNMANGLED_H

#include <iostream> // for std::cout
#include <mutex> // for std::mutex
#include <sstream> // for std::ostringstream

struct PrintUnmangled
{
  using Ioo = std::ostream &(*)(std::ostream &);
  using Ios = std::ostream &(*)(std::ios &);
  using Iob = std::ostream &(*)(std::ios_base &);

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
  PrintUnmangled &operator<<(T const &value);

  PrintUnmangled &operator<<(Ioo value);
  PrintUnmangled &operator<<(Ios value);
  PrintUnmangled &operator<<(Iob value);

private:
  static std::mutex &mutex();
  PrintUnmangled &cleared();

private:
  std::ostream &out;
  std::ostringstream oss;
};

template<typename T>
PrintUnmangled &PrintUnmangled::operator<<(T const &value)
{
  oss << value;
  return *this;
}

inline PrintUnmangled &PrintUnmangled::operator<<(Ioo value)
{
  {
    std::lock_guard<std::mutex> lock(mutex());
    out << oss.str() << value;
  }
  return cleared();
}

inline PrintUnmangled &PrintUnmangled::operator<<(Ios value)
{
  {
    std::lock_guard<std::mutex> lock(mutex());
    out << oss.str() << value;
  }
  return cleared();
}

inline PrintUnmangled &PrintUnmangled::operator<<(Iob value)
{
  {
    std::lock_guard<std::mutex> lock(mutex());
    out << oss.str() << value;
  }
  return cleared();
}

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

#endif // PRINT_UNMANGLED_H

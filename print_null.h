#ifndef PRINT_NULL_H
#define PRINT_NULL_H

#include <iostream> // for std::ostream

struct PrintNull
{
  template<typename T>
  friend PrintNull &operator<<(PrintNull &os, T const &);

  using Ioo = std::ostream &(*)(std::ostream &);
  friend PrintNull &operator<<(PrintNull &os, Ioo const &);

  using Ios = std::ostream &(*)(std::ios &);
  friend PrintNull &operator<<(PrintNull &os, Ios const &);

  using Iob = std::ostream &(*)(std::ios_base &);
  friend PrintNull &operator<<(PrintNull &os, Iob const &);
};

template<typename T>
PrintNull &operator<<(PrintNull &os, T const &)
{
  return os;
}

inline PrintNull &operator<<(PrintNull &os, PrintNull::Ioo const &)
{
  return os;
}

inline PrintNull &operator<<(PrintNull &os, PrintNull::Ios const &)
{
  return os;
}

inline PrintNull &operator<<(PrintNull &os, PrintNull::Iob const &)
{
  return os;
}

#endif // PRINT_NULL_H

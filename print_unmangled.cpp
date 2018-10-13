#include "print_unmangled.h"

std::mutex PrintUnmangled::mtx{};

PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Ioo const &value)
{
  std::lock_guard<std::mutex> lock(os.mtx);
  std::cout << value;
  return os;
}

PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Ios const &value)
{
  std::lock_guard<std::mutex> lock(os.mtx);
  std::cout << value;
  return os;
}

PrintUnmangled &operator<<(PrintUnmangled &os, PrintUnmangled::Iob const &value)
{
  std::lock_guard<std::mutex> lock(os.mtx);
  std::cout << value;
  return os;
}

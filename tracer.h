#ifndef TRACER_H
#define TRACER_H

#include <iostream>
#include <mutex>
#include <string>

struct Tracer
{
  static std::mutex mtx;
  static unsigned count;
  std::string name;

  Tracer()
  {
    std::unique_lock<std::mutex> lock(mtx);
    name = "unnamed_" + std::to_string(count++);
    std::cout << "Tracer '" << name << "' created" << std::endl;
  }

  explicit Tracer(char const *name)
    : name(name)
  {
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Tracer '" << name << "' created" << std::endl;
  }

  Tracer(Tracer const &other)
  {
    std::unique_lock<std::mutex> lock(mtx);
    name = other.name + "_cc_" + std::to_string(count++);
    std::cout << "Tracer '" << name << "' "
      "copy-constructed from '" << other.name << "'" << std::endl;
  }

  Tracer(Tracer &&other)
    : name(other.name)
  {
    std::unique_lock<std::mutex> lock(mtx);
    other.name += "_mc_" + std::to_string(count++);
    std::cout << "Tracer '" << name << "' "
      "move-constructed from '" << other.name << "'" << std::endl;
  }

  ~Tracer()
  {
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Tracer '" << name << "' destroyed" << std::endl;
  }

  Tracer &operator=(Tracer const &other)
  {
    std::unique_lock<std::mutex> lock(mtx);
    name = other.name + "_ca_" + std::to_string(count++);
    std::cout << "Tracer '" << name << "' "
      "copy-assigned from '" << other.name << "'" << std::endl;
    return *this;
  }

  Tracer &operator=(Tracer &&other)
  {
    std::unique_lock<std::mutex> lock(mtx);
    name = std::move(other.name);
    other.name = name + "_ma_" + std::to_string(count++);
    std::cout << "Tracer '" << name << "' "
      "move-assigned from '" << other.name << "'" << std::endl;
    return *this;
  }
};

unsigned Tracer::count = 0;
std::mutex Tracer::mtx;

#endif // TRACER_H

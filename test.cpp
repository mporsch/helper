#include "helper.h"
#include "tracer.h"
#include "work_queue.h"

#include <iostream>
#include <thread>
#include <string>
#include <tuple>

int count()
{
  static int counter = 0;
  return ++counter;
}

std::string text(int seconds)
{
  return "Sleeping for " + std::to_string(seconds) + " sec";
}

void sleep(int seconds)
{
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

int main(int /*argc*/, char **/*argv*/) {
  std::tuple<char, int, float, Tracer> t1{'a', 2, 0.11f, "tuple_1_element"};
  std::tuple<char, int, float, Tracer, std::string> t12 = std::tuple_cat(t1, std::tie("text"));

  std::tuple<char, int, float, Tracer> t2{'a', 2, 0.11f, "tuple_2_element"};
  std::tuple<char, int, float, Tracer, std::string> t21 = std::tuple_cat(std::move(t2), std::tie("text"));

  std::cout << "IsAnyEqual(0, 1, 2, 3) " << std::boolalpha
    << "-> " << IsAnyEqual(0, 1, 2, 3) << std::endl;
  std::cout << "IsAnyEqual(4, 4, 5, 6) " << std::boolalpha
    << "-> " << IsAnyEqual(4, 4, 5, 6) << std::endl;
  std::cout << "IsAnyEqual(7, 8, 9, 7) " << std::boolalpha
    << "-> " << IsAnyEqual(7, 8, 9, 7) << std::endl;

  std::cout << "IsAnyEqual(std::string('one'), std::string('two'), std::string('three')) " << std::boolalpha
    << "-> " << IsAnyEqual(std::string("one"), std::string("two"), std::string("three")) << std::endl;
  std::cout << "IsAnyEqual(std::string('four'), std::string('four'), std::string('five')) " << std::boolalpha
    << "-> " << IsAnyEqual(std::string("four"), std::string("four"), std::string("five")) << std::endl;
  std::cout << "IsAnyEqual(std::string('six'), std::string('seven'), std::string('six')) " << std::boolalpha
    << "-> " << IsAnyEqual(std::string("six"), std::string("seven"), std::string("six")) << std::endl;

  WorkQueue workQueue;

  int seconds = workQueue.Assign(count).get();
  std::cout << workQueue.Assign(text, seconds).get() << std::endl;
  workQueue.Assign(sleep, seconds).wait();

  seconds = workQueue.Assign(
    []() -> int
    {
      return count();
    }).get();
  std::cout << workQueue.Assign(std::bind(
    [](int seconds) -> std::string
    {
      return text(seconds);
    }, seconds)).get() << std::endl;
  workQueue.Assign(
    [](int seconds)
    {
      sleep(seconds);
    }, seconds).wait();

  return EXIT_SUCCESS;
}

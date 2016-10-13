#include <iostream>
#include "helper.h"

int main(int argc, char **argv) {
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
}


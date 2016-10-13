#include <iostream>
#include "helper.h"

int main(int argc, char **argv) {
  std::cout << "IsAnyEqual(0, 1, 2, 3) " << std::boolalpha
    << "-> " << IsAnyEqual(0, 1, 2, 3) << std::endl;
  std::cout << "IsAnyEqual(4, 4, 5, 6) " << std::boolalpha
    << "-> " << IsAnyEqual(4, 4, 5, 6) << std::endl;
  std::cout << "IsAnyEqual(7, 8, 9, 7) " << std::boolalpha
    << "-> " << IsAnyEqual(7, 8, 9, 7) << std::endl;
}

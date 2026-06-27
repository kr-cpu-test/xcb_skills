#include "hello_bench/hello_bench.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
  const std::string result = hello_bench::format_hello("world");
  if (result != "hello world") {
    std::cerr << "unexpected output: " << result << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

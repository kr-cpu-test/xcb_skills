#include "hello_bench/hello_bench.hpp"

#include <string>

namespace hello_bench {

std::string format_hello(std::string_view name) {
  return "hello " + std::string(name);
}

}

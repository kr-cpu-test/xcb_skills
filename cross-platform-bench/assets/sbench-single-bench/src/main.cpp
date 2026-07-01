#include "hello_bench/hello_bench.hpp"
#include "sbench/sbench.hpp"

#include <string>

class HelloBench : public MicroBench {
public:
  HelloBench() : MicroBench("hello") {
    app->description("Hello world smoke bench");
    app->add_option("--name", name_, "Name to print");
    app->callback([this]() { println("{}", hello_bench::format_hello(name_)); });
  }

private:
  std::string name_ = "world";
};

int main(int argc, char **argv) {
  HelloBench bench;
  return bench.main(argc, argv);
}

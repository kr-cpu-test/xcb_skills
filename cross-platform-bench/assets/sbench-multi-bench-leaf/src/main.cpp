#include "hello_bench/hello_bench.hpp"
#include "sbench/register.hpp"
#include "sbench/sbench.hpp"

#include <string>

SBENCH_BENCH_FACTORY(hello_bench_factory);

class HelloBench : public MicroBench {
public:
  HelloBench() : MicroBench("hello_bench") {
    app->description("Hello world smoke bench");
    app->add_option("--name", name_, "Name to print");
    app->callback([this]() { println("{}", hello_bench::format_hello(name_)); });
  }

private:
  std::string name_ = "world";
};

SBENCH_BENCH_REGISTER_INTO(HelloBench, hello_bench_factory);

class HelloBenchSuite : public ForkableBench {
public:
  HelloBenchSuite() : ForkableBench(hello_bench_factory_get_instance()) {
    app->name("hello_bench");
    app->description("Multi-bench leaf hello world example");
  }

protected:
  std::unique_ptr<ForkableBench> create_fork_instance() const override {
    return std::make_unique<HelloBenchSuite>();
  }
};

SBENCH_SUITE_ENTRY(HelloBenchSuite);

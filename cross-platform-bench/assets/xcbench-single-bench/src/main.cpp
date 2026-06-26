#include "hello_bench/hello_bench.hpp"
#include "sbench/sbench.hpp"
#include "xcb_single/register.hpp"

#include <string>

MICRO_BENCH_FACTORY(xcb_suite_factory);

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

XCB_BENCH_REGISTER(HelloBench);

class HelloSuite : public ForkableBench {
public:
  HelloSuite() : ForkableBench(xcb_suite_factory_get_instance()) {
    app->name("hello_bench");
    app->description("Single-bench hello world example");
  }

protected:
  std::unique_ptr<ForkableBench> create_fork_instance() const override {
    return std::make_unique<HelloSuite>();
  }
};

XCB_BENCH_MAIN_ENTRY(HelloSuite);

#include <sstream>

#include "unittest.h"

using namespace testlib;

class this_test_passes : public test
{
  void Run() override {}
};

class this_test_fails : public test
{
  void Run() override { Passed = false; }
};

TEST_CASE(Cannot_run_0_tests)
{
  std::stringstream ErrorStream;
  test_runner Runner({}, ErrorStream);

  const auto Result = Runner.Run();

  ASSERT(Result == false);
  ASSERT(ErrorStream.str().contains(test_runner::ZERO_TESTS_ERROR_MSG));
}

TEST_CASE(Can_run_mulitple_passing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  std::stringstream ErrorStream;
  auto Runner = test_runner(std::move(TestCases), ErrorStream);

  ASSERT(Runner.Run() == true);
  ASSERT(ErrorStream.str().contains("Passed 2/2 tests"));
}

TEST_CASE(Can_run_passing_and_failing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_fails>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  std::stringstream ErrorStream;
  auto Runner = test_runner(std::move(TestCases), ErrorStream);

  ASSERT(Runner.Run() == false);
  ASSERT(ErrorStream.str().contains("Passed 2/3 tests"));
}

TEST_CASE(Test_metrics_are_not_valid_before_running_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_fails>());

  auto Runner = test_runner(std::move(TestCases));

  // Oops, forgot to run the tests!
  // ASSERT(Runner.Run() == false);

  ASSERT(Runner.TestsPassed() == static_cast<size_t>(-1));
  ASSERT(Runner.TestsRun() == static_cast<size_t>(-1));
  ASSERT(Runner.TestsFailed() == static_cast<size_t>(-1));
}

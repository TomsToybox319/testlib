#include <sstream>

#include "unittest.h"

using namespace testlib;

class this_test_passes : public test
{
 public:
  this_test_passes() : test("this_test_passes", __FILE__) {}
  void RunImpl(std::ostream&) override {}
};

class this_test_fails : public test
{
 public:
  this_test_fails() : test("this_test_fails", __FILE__) {}
  void RunImpl(std::ostream& TestImpl_Stream) override { ASSERT(1 == 2); }
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

TEST_CASE(Test_reports_name_and_status)
{
  this_test_passes PassingTest;
  std::stringstream ErrorStream;
  PassingTest.Run(ErrorStream);
  ASSERT(
      ErrorStream.str().contains("test_utest.cpp::this_test_passes - PASSED"));

  this_test_fails FailingTest;
  ErrorStream.clear();
  FailingTest.Run(ErrorStream);
  ASSERT(ErrorStream.str().contains(
      "test_utest.cpp::this_test_fails - FAILED\n1 == 2"));
}


#include "unittest.h"

using namespace testlib;

class this_test_passes : public test
{
 public:
  constexpr this_test_passes() : test("this_test_passes", __FILE__) {}
  constexpr void RunImpl() const override {}
};

class this_test_fails : public test
{
 public:
  constexpr this_test_fails() : test("this_test_fails", __FILE__) {}
  void RunImpl() const override
  {
    ASSERT(1 == 2);
    ASSERT(2 == 3);
  }
};

TEST_CASE(Cannot_run_0_tests)
{
  test_runner Runner({});

  const auto Result = Runner.Run();

  ASSERT(Result.Passed == false);
  ASSERT(Result.Message.contains(test_runner::ZERO_TESTS_ERROR_MSG));
}

TEST_CASE(Can_run_mulitple_passing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  auto Runner = test_runner(std::move(TestCases));
  const auto Result = Runner.Run();

  ASSERT(Result.Passed == true);
  ASSERT(Result.Message.contains("Passed 2/2 tests"));
}

TEST_CASE(Can_run_passing_and_failing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_fails>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  auto Runner = test_runner(std::move(TestCases));
  const auto Result = Runner.Run();

  ASSERT(Result.Passed == false);
  ASSERT(Result.Message.contains("Passed 2/3 tests"));
}

TEST_CASE(Test_reports_name_and_status)
{
  {
    const this_test_passes PassingTest;
    const auto Result = PassingTest.Run();
    ASSERT(
        Result.Message.contains("test_utest.cpp::this_test_passes - PASSED"));
  }
  {
    const this_test_fails FailingTest;
    const auto Result = FailingTest.Run();
    ASSERT(Result.Message.contains(
        "test_utest.cpp::this_test_fails - FAILED\n1 == 2"));
  }
}

TEST_CASE(Test_only_reports_first_failure)
{
  const this_test_fails FailingTest;
  const auto Result = FailingTest.Run();
  ASSERT(Result.Message.contains(
      "test_utest.cpp::this_test_fails - FAILED\n1 == 2"));
  ASSERT(!Result.Message.contains("2 == 3"));
}


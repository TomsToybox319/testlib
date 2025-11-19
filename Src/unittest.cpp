#include "unittest.h"

#include <format>
#include <numeric>

using namespace testlib;

namespace
{

constexpr test_runner::result GuardAgainstEmptyTests(
    const std::vector<std::unique_ptr<test>>& TestCases)
{
  return TestCases.empty() ? test_runner::result(
                                 false, test_runner::ZERO_TESTS_ERROR_MSG, 0, 0)
                           : test_runner::result();
}

std::string WriteReport(const test_runner::result& Result)
{
  return std::format("Passed {}/{} tests\n", Result.TestsPassed,
                     Result.TestsRun());
}

constexpr test_runner::result Finalize(const test_runner::result& Result)
{
  test_runner::result FinalResult = Result;
  FinalResult.Message += WriteReport(Result);
  return FinalResult;
}
}  // namespace

void test::AssertImpl(bool Passed, const char* Expr, int Line,
                      const char* Macro, bool ThrowOnFail)
{
  const auto Message =
      Passed ? ""
             : std::format("{}({}) failed on line {}\n", Macro, Expr, Line);
  TestImpl_Result = TestImpl_Result + result{Passed, Message};
  if (ThrowOnFail && !Passed)
  {
    throw assertion_error();
  }
}

test::result test::Run()
{
  try
  {
    RunImpl();
  }
  catch (const assertion_error&)
  {
  }

  const char* const ResultStr = TestImpl_Result.Passed ? "PASSED" : "FAILED";
  const auto Message = std::format("{}::{} - {}\n{}", Filename, Name, ResultStr,
                                   TestImpl_Result.Message);
  return {TestImpl_Result.Passed, Message};
}

test_runner::result test_runner::Run()
{
  const auto GuardResult = GuardAgainstEmptyTests(mTestCases);
  if (!GuardResult.Passed) return GuardResult;

  const auto Result = std::accumulate(
      mTestCases.begin(), mTestCases.end(), result(),
      [](const result& Result, const std::unique_ptr<test>& Test)
      { return Result + Test->Run(); });

  return Finalize(Result);
}

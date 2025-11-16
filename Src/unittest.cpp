#include "unittest.h"

#include <cassert>
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

test::result test::Run() const
{
  std::string Message{};
  bool Passed = true;

  try
  {
    RunImpl();
  }
  catch (const assertion_error& Error)
  {
    Passed = false;
    Message = std::format("{} failed on line {}\n", Error.Expr, Error.Line);
  }

  const char* const ResultStr = Passed ? "PASSED" : "FAILED";
  Message = std::format("{}::{} - {}\n{}", Filename, Name, ResultStr, Message);
  return {Passed, Message};
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

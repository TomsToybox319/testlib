#include "unittest.h"

#include <cassert>
#include <format>
#include <numeric>

using namespace testlib;

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

std::string test_runner::WriteReport(const result& Result) const
{
  return std::format("Passed {}/{} tests\n", Result.TestsPassed,
                     Result.TestsRun);
}

test_runner::result test_runner::Run()
{
  const auto GuardResult = GuardAgainstEmptyTests();
  if (!GuardResult.Passed) return GuardResult;

  const auto Result = std::accumulate(
      mTestCases.begin(), mTestCases.end(), result{true, ""},
      [](const result& Result, const std::unique_ptr<test>& Test)
      { return Result + Test->Run(); });

  return {Result.TestsFailed == 0, Result.Message + WriteReport(Result)};
}

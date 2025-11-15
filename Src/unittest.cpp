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

std::string test_runner::WriteReport() const
{
  return std::format("Passed {}/{} tests\n", TestsPassed(), TestsRun());
}

test_runner::result test_runner::RunSingleTest(const test& Test)
{
  const auto [Passed, Message] = Test.Run();

  mTestsRun++;
  if (Passed)
  {
    mTestsPassed++;
  }
  else
  {
    mTestsFailed++;
  }
  return {Passed, Message};
}

test_runner::result test_runner::Run()
{
  const auto GuardResult = GuardAgainstEmptyTests();
  if (!GuardResult.Passed) return GuardResult;

  mTestsRun = 0;
  mTestsFailed = 0;
  mTestsPassed = 0;

  result Result = {true, ""};
  for (const auto& Test : mTestCases)
  {
    assert(Test.get() != nullptr);
    Result = Result + RunSingleTest(*Test);
  }

  return {mTestsFailed == 0, Result.Message + WriteReport()};
}

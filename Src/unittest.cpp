#include "unittest.h"

#include <cassert>
#include <format>
#include <numeric>
#include <sstream>

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

bool test_runner::GuardAgainstEmptyTests() const
{
  const bool RunnerHasTests = !mTestCases.empty();
  if (!RunnerHasTests) mErrorStream << ZERO_TESTS_ERROR_MSG << std::endl;
  return RunnerHasTests;
}

std::string test_runner::WriteReport() const
{
  return std::format("Passed {}/{} tests", TestsPassed(), TestsRun());
}

void test_runner::RunSingleTest(const test& Test)
{
  const auto [Passed, Message] = Test.Run();
  mErrorStream << Message;

  mTestsRun++;
  if (Passed)
  {
    mTestsPassed++;
  }
  else
  {
    mTestsFailed++;
  }
}

bool test_runner::Run()
{
  if (!GuardAgainstEmptyTests()) return false;

  mTestsRun = 0;
  mTestsFailed = 0;
  mTestsPassed = 0;

  for (const auto& Test : mTestCases)
  {
    assert(Test.get() != nullptr);
    RunSingleTest(*Test);
  }

  mErrorStream << WriteReport() << "\n";
  return mTestsFailed == 0;
}

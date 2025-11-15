#include "unittest.h"

#include <cassert>
#include <format>
#include <numeric>
#include <sstream>

using namespace testlib;

void test::Run(std::ostream& Stream)
{
  std::string Message{};

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
  Stream << std::format("{}::{} - {}\n{}", Filename, Name, ResultStr, Message);
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

size_t test_runner::TestsPassed() const { return mTestsPassed; }

size_t test_runner::TestsFailed() const { return mTestsFailed; }

size_t test_runner::TestsRun() const { return mTestsRun; }

void test_runner::RunSingleTest(test& Test)
{
  Test.Run(mErrorStream);

  mTestsRun++;
  if (!Test.Passed)
  {
    mTestsFailed++;
  }
  else
  {
    mTestsPassed++;
  }
}

bool test_runner::Run()
{
  if (!GuardAgainstEmptyTests()) return false;

  mTestsRun = 0;
  mTestsFailed = 0;
  mTestsPassed = 0;

  for (auto& Test : mTestCases)
  {
    assert(Test.get() != nullptr);
    RunSingleTest(*Test);
  }

  mErrorStream << WriteReport() << "\n";
  return mTestsFailed == 0;
}

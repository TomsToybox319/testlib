#include "unittest.h"

#include <format>
#include <numeric>

using namespace testlib;

void test::Run(std::ostream& Stream)
{
  Stream << std::format("Running {} - ", Name);
  RunImpl();
  const char* const ResultStr = Passed ? "PASSED" : "FAILED";
  Stream << std::format("{}\n", ResultStr);
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

bool test_runner::Run()
{
  if (!GuardAgainstEmptyTests()) return false;

  bool Result = true;
  mTestsRun = 0;
  mTestsFailed = 0;
  mTestsPassed = 0;

  for (auto& Test : mTestCases)
  {
    Test->Run(mErrorStream);
    mTestsRun++;
    if (!Test->Passed)
    {
      Result = false;
      mTestsFailed++;
    }
    else
    {
      mTestsPassed++;
    }
  }

  mErrorStream << WriteReport() << "\n";
  return Result;
}

#ifndef TOM_UNITTEST_H
#define TOM_UNITTEST_H

#include <iostream>
#include <memory>
#include <vector>

namespace testlib
{
class test
{
 public:
  virtual ~test() = default;
  virtual void Run() = 0;
  bool Passed = true;
};

extern std::vector<std::unique_ptr<test>> Tests;

class test_runner
{
 public:
  static constexpr const char ZERO_TESTS_ERROR_MSG[] =
      "TestRunner found no tests to run.";

  test_runner(std::vector<std::unique_ptr<test>>&& TestCases,
              std::ostream& ErrorStream = std::cerr)
      : mTestCases(std::move(TestCases)), mErrorStream(ErrorStream)
  {
  }

  bool Run();
  std::string WriteReport() const;

  size_t TestsPassed() const;
  size_t TestsFailed() const;
  size_t TestsRun() const;

 private:
  bool GuardAgainstEmptyTests() const;
  std::vector<std::unique_ptr<test>> mTestCases;
  std::ostream& mErrorStream;

  size_t mTestsFailed = static_cast<size_t>(-1);
  size_t mTestsRun = static_cast<size_t>(-1);
  size_t mTestsPassed = static_cast<size_t>(-1);
};
}  // namespace testlib

#define TEST_CASE(TestName)                                                  \
  class testlib_##TestName : public testlib::test                            \
  {                                                                          \
   public:                                                                   \
    void Run() override;                                                     \
  };                                                                         \
  namespace                                                                  \
  {                                                                          \
  struct testlib_registrar_##TestName                                        \
  {                                                                          \
    testlib_registrar_##TestName()                                           \
    {                                                                        \
      testlib::Tests.push_back(std::make_unique<testlib_##TestName>());      \
    }                                                                        \
  };                                                                         \
  static testlib_registrar_##TestName testlib_registrar_instance_##TestName; \
  }                                                                          \
  void testlib_##TestName::Run()

#define ASSERT(Expr)                                              \
  if (!(Expr))                                                    \
  {                                                               \
    Passed = false;                                               \
    std::cerr << #Expr << " failed on line " << __LINE__ << "\n"; \
  }

#define UNIT_TEST_INIT                                      \
  namespace testlib                                         \
  {                                                         \
  std::vector<std::unique_ptr<test>> Tests;                 \
  }                                                         \
  int main()                                                \
  {                                                         \
    testlib::test_runner Runner(std::move(testlib::Tests)); \
    Runner.Run();                                           \
    return static_cast<int>(Runner.TestsFailed());          \
  }

#endif

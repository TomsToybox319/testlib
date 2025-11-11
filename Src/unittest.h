#ifndef TOM_UNITTEST_H
#define TOM_UNITTEST_H

#include <iostream>
#include <memory>
#include <vector>

#ifdef UNIT_TEST_INIT
#error "UNIT_TEST_INIT has already been defined!"
#endif

#ifdef TEST_CASE
#error "TEST_CASE has already been defined!"
#endif

#ifdef ASSERT
#error "ASSERT has already been defined!"
#endif

namespace testlib
{
class test
{
 public:
  virtual ~test() = default;
  virtual void Run() = 0;
  bool Passed = true;
};

// This contains all the tests, which are implicitly registered in their
// custructor
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

  // Test statistics. Run() initializes them as the tests are run.
  size_t mTestsFailed = static_cast<size_t>(-1);
  size_t mTestsRun = static_cast<size_t>(-1);
  size_t mTestsPassed = static_cast<size_t>(-1);
};
}  // namespace testlib

// This is where it gets complicated. Each test case is a sub-class of test.
// We also need to register it by adding it to the Tests vector.
// This is tricky because we're at top-level scope.
// The solution is to create a static instance of a struct whose ctor
// appends an instance of the associated test to the vector.
// (Maybe at some point a wizard can figure out how we can get this without the
// extra adding-struct.)
// Finally, we end with the signature of the run function, so the caller can
// treat this as a method and write the test afterward
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

// Because of how things are called, it doesn't make sense to "return"
// anything here. Instead we set a member variable to tell whether it
// passed. In the future, we'll probably need an exception so we can abort
// execution at the first failure
#define ASSERT(Expr)                                              \
  if (!(Expr))                                                    \
  {                                                               \
    Passed = false;                                               \
    std::cerr << #Expr << " failed on line " << __LINE__ << "\n"; \
  }

// This wraps main
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

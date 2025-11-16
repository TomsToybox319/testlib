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
  struct result
  {
    const bool Passed;
    const std::string Message;
  };

  constexpr test(const char* Name, const char* Filename)
      : Name(Name), Filename(Filename)
  {
  }
  constexpr virtual ~test() = default;
  result Run() const;

  const char* const Name;
  const char* const Filename;

 protected:
  constexpr virtual void RunImpl() const = 0;
};

class assertion_error
{
 public:
  assertion_error(const char* Expr, int Line) : Expr(Expr), Line(Line) {}
  const char* const Expr;
  const int Line;
};

// This contains all the tests, which are implicitly registered in their
// custructor
extern std::vector<std::unique_ptr<test>> Tests;

class test_runner
{
 public:
  struct result
  {
    bool Passed;
    std::string Message;
    size_t TestsRun = 0;
    size_t TestsFailed = 0;
    size_t TestsPassed = 0;

    constexpr result() : Passed(true) {}

    constexpr result(const test::result& Rhs)
        : Passed(Rhs.Passed),
          Message(Rhs.Message),
          TestsRun(1),
          TestsFailed(!Rhs.Passed),
          TestsPassed(Rhs.Passed)
    {
    }

    constexpr result(bool Passed, std::string Message, size_t TestsRun,
                     size_t TestsFailed, size_t TestsPassed)
        : Passed(Passed),
          Message(Message),
          TestsRun(TestsRun),
          TestsFailed(TestsFailed),
          TestsPassed(TestsPassed)
    {
    }

    constexpr result operator+(const result& Rhs) const
    {
      return result(Passed && Rhs.Passed, Message + Rhs.Message,
                    TestsRun + Rhs.TestsRun, TestsFailed + Rhs.TestsFailed,
                    TestsPassed + Rhs.TestsPassed);
    }

    constexpr result operator+(const test::result& Rhs) const
    {
      return *this + result(Rhs);
    }
  };
  static constexpr const char ZERO_TESTS_ERROR_MSG[] =
      "TestRunner found no tests to run.\n";

  constexpr test_runner(std::vector<std::unique_ptr<test>>&& TestCases)
      : mTestCases(std::move(TestCases))
  {
  }

  result Run();

 private:
  constexpr result GuardAgainstEmptyTests() const
  {
    const bool RunnerHasTests = !mTestCases.empty();
    return RunnerHasTests ? result(true, "", 0, 0, 0)
                          : result(false, ZERO_TESTS_ERROR_MSG, 0, 0, 0);
  }
  std::vector<std::unique_ptr<test>> mTestCases;
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
    constexpr testlib_##TestName() : test(#TestName, __FILE__) {}            \
    void RunImpl() const override;                                           \
  };                                                                         \
  namespace                                                                  \
  {                                                                          \
  struct testlib_registrar_##TestName                                        \
  {                                                                          \
    constexpr testlib_registrar_##TestName()                                 \
    {                                                                        \
      testlib::Tests.push_back(std::make_unique<testlib_##TestName>());      \
    }                                                                        \
  };                                                                         \
  static testlib_registrar_##TestName testlib_registrar_instance_##TestName; \
  }                                                                          \
  void testlib_##TestName::RunImpl() const

// Because of how things are called, it doesn't make sense to "return"
// anything here. Instead we set a member variable to tell whether it
// passed. In the future, we'll probably need an exception so we can abort
// execution at the first failure
#define ASSERT(Expr)                        \
  if (!(Expr))                              \
  {                                         \
    throw assertion_error(#Expr, __LINE__); \
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
    const auto Report = Runner.Run();                       \
    std::cerr << Report.Message << "\n";                    \
    return static_cast<int>(Report.TestsFailed);            \
  }

#endif

#ifndef TOM_UNITTEST_H
#define TOM_UNITTEST_H

#include <concepts>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#ifdef UNIT_TEST_INIT
#error "UNIT_TEST_INIT has already been defined!"
#endif

#ifdef TEST
#error "TEST has already been defined!"
#endif

#ifdef ASSERT
#error "ASSERT has already been defined!"
#endif

#ifdef ASSERT_FALSE
#error "ASSERT_FALSE has already been defined!"
#endif

#ifdef EXPECT
#error "EXPECT has already been defined!"
#endif

#ifdef EXPECT_FALSE
#error "EXPECT_FALSE has already been defined!"
#endif

#ifdef ASSERT_EQ
#error "ASSERT_EQ has already been defined!"
#endif

#ifdef EXPECT_EQ
#error "EXPECT_EQ has already been defined!"
#endif

#ifdef EXPECT_NE
#error "EXPECT_NE has already been defined!"
#endif

#ifdef ASSERT_NE
#error "ASSERT_NE has already been defined!"
#endif

namespace testlib
{
class assertion_error
{
 public:
  constexpr assertion_error() = default;
};

class test
{
 public:
  struct result
  {
    std::string Message;
    bool Passed = true;

    constexpr result operator+(const result& Rhs) const
    {
      return {Passed && Rhs.Passed, Message + Rhs.Message};
    }
  };

  constexpr test(const char* NameParam, const char* FilenameParam)
      : Name(NameParam), Filename(FilenameParam)
  {
  }
  constexpr virtual ~test() = default;
  result Run();

  const char* const Name;
  const char* const Filename;

 protected:
  template <typename T, typename U>
    requires std::equality_comparable_with<T, U>
  void AssertEqImpl(const char* LhsName, const char* RhsName, int Line,
                    const char* Macro, bool ThrowOnFail, T Lhs, U Rhs,
                    bool ExpectedValue = true)
  {
    const bool Passed = (Lhs == Rhs) == ExpectedValue;
    const auto Message =
        Passed ? ""
               : std::format(
                     "{}({}, {}) failed on line {}.\n  Lhs: {}\n  Rhs: {}\n",
                     Macro, LhsName, RhsName, Line, Lhs, Rhs);
    TestImpl_Result = TestImpl_Result + result{Passed, Message};
    if (ThrowOnFail && !Passed)
    {
      throw assertion_error();
    }
  }

  void AssertImpl(bool Passed, const char* Expr, int Line, const char* Macro,
                  bool ThrowOnFail);
  constexpr virtual void RunImpl() = 0;
  result TestImpl_Result;
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
    size_t TestsFailed = 0;
    size_t TestsPassed = 0;
    constexpr size_t TestsRun() const { return TestsFailed + TestsPassed; }

    constexpr result() : Passed(true) {}

    // Convert test:;result (individual) to test_runner::result (aggregate)
    constexpr result(const test::result& Rhs)
        : Passed(Rhs.Passed),
          Message(Rhs.Message),
          TestsFailed(!Rhs.Passed),
          TestsPassed(Rhs.Passed)
    {
    }

    constexpr result(bool Passed, std::string Message, size_t TestsFailed,
                     size_t TestsPassed)
        : Passed(Passed),
          Message(Message),
          TestsFailed(TestsFailed),
          TestsPassed(TestsPassed)
    {
    }

    constexpr result operator+(const result& Rhs) const
    {
      return result(Passed && Rhs.Passed, Message + Rhs.Message,
                    TestsFailed + Rhs.TestsFailed,
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
#define TEST(TestName)                                                       \
  class testlib_##TestName : public testlib::test                            \
  {                                                                          \
   public:                                                                   \
    constexpr testlib_##TestName() : test(#TestName, __FILE__) {}            \
    void RunImpl() override;                                                 \
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
  void testlib_##TestName::RunImpl()

// Because of how things are called, it doesn't make sense to "return"
// anything here. Instead we set a member variable to tell whether the test
// passed.
#define ASSERT(Expr) AssertImpl((Expr), #Expr, __LINE__, "ASSERT", true)

#define ASSERT_FALSE(Expr) \
  AssertImpl(!(Expr), #Expr, __LINE__, "ASSERT_FALSE", true)

#define EXPECT(Expr) AssertImpl((Expr), #Expr, __LINE__, "EXPECT", false)

#define EXPECT_FALSE(Expr) \
  AssertImpl(!(Expr), #Expr, __LINE__, "EXPECT_FALSE", false)

#define ASSERT_EQ(Lhs, Rhs) \
  AssertEqImpl(#Lhs, #Rhs, __LINE__, "ASSERT_EQ", true, Lhs, Rhs)

#define EXPECT_EQ(Lhs, Rhs) \
  AssertEqImpl(#Lhs, #Rhs, __LINE__, "EXPECT_EQ", false, Lhs, Rhs)

#define ASSERT_NE(Lhs, Rhs) \
  AssertEqImpl(#Lhs, #Rhs, __LINE__, "ASSERT_NE", true, Lhs, Rhs, false)

#define EXPECT_NE(Lhs, Rhs) \
  AssertEqImpl(#Lhs, #Rhs, __LINE__, "EXPECT_NE", false, Lhs, Rhs, false)

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

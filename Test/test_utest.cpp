#include "unittest.h"

using namespace testlib;

class this_test_passes : public test
{
 public:
  constexpr this_test_passes() : test("this_test_passes", __FILE__) {}
  constexpr void RunImpl() override {}
};

class this_test_fails : public test
{
 public:
  constexpr this_test_fails() : test("this_test_fails", __FILE__) {}
  void RunImpl() override
  {
    ASSERT_FALSE(1 == 2);
    ASSERT(1 == 2);
    ASSERT(2 == 3);
  }
};

class assert_false_failure : public test
{
 public:
  constexpr assert_false_failure() : test("assert_false_failure", __FILE__) {}
  void RunImpl() override { ASSERT_FALSE(true); }
};

class expect_assertions : public test
{
 public:
  constexpr expect_assertions() : test("expect_assertions", __FILE__) {}
  void RunImpl() override
  {
    EXPECT(1 == 2);
    EXPECT(2 == 3);
    EXPECT(2 == 2);
    EXPECT_FALSE(1 == 1);
  }
};

class assert_eq : public test
{
 public:
  constexpr assert_eq() : test("assert_eq", __FILE__) {}
  void RunImpl() override
  {
    EXPECT_EQ(1, 1);
    EXPECT_EQ(1, 2);

    ASSERT_EQ(1, 1);
    ASSERT_EQ(1, 2);
    ASSERT_EQ(2, 3);
  }
};

class assert_ne : public test
{
 public:
  constexpr assert_ne() : test("assert_ne", __FILE__) {}
  void RunImpl() override
  {
    EXPECT_NE(1, 2);
    EXPECT_NE(1, 1);

    ASSERT_NE(1, 2);
    ASSERT_NE(1, 1);
    ASSERT_NE(2, 2);
  }
};

TEST(Cannot_run_0_tests)
{
  test_runner Runner({});

  const auto Result = Runner.Run();

  ASSERT(Result.Passed == false);
  ASSERT(Result.Message.contains(test_runner::ZERO_TESTS_ERROR_MSG));
}

TEST(Can_run_mulitple_passing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  auto Runner = test_runner(std::move(TestCases));
  const auto Result = Runner.Run();

  ASSERT(Result.Passed == true);
  ASSERT(Result.Message.contains("Passed 2/2 tests"));
}

TEST(Can_run_passing_and_failing_tests)
{
  std::vector<std::unique_ptr<test>> TestCases;
  TestCases.push_back(std::make_unique<this_test_passes>());
  TestCases.push_back(std::make_unique<this_test_fails>());
  TestCases.push_back(std::make_unique<this_test_passes>());

  auto Runner = test_runner(std::move(TestCases));
  const auto Result = Runner.Run();

  ASSERT(Result.Passed == false);
  ASSERT(Result.Message.contains("Passed 2/3 tests"));
}

TEST(Test_reports_name_and_status)
{
  {
    this_test_passes PassingTest;
    const auto Result = PassingTest.Run();
    ASSERT(
        Result.Message.contains("test_utest.cpp::this_test_passes - PASSED"));
  }
  {
    this_test_fails FailingTest;
    const auto Result = FailingTest.Run();
    ASSERT(Result.Message.contains(
        "test_utest.cpp::this_test_fails - FAILED\nASSERT(1 == 2)"));
  }
}

TEST(Test_only_reports_first_failure)
{
  this_test_fails FailingTest;
  const auto Result = FailingTest.Run();
  ASSERT(Result.Message.contains(
      "test_utest.cpp::this_test_fails - FAILED\nASSERT(1 == 2)"));
  ASSERT_FALSE(Result.Message.contains("2 == 3"));
}

TEST(Multiple_expects_can_fail)
{
  expect_assertions FailingTest;
  const auto Result = FailingTest.Run();
  EXPECT(Result.Message.contains("EXPECT(1 == 2) failed on line"));
  EXPECT(Result.Message.contains("EXPECT(2 == 3) failed on line"));
  EXPECT(!Result.Message.contains("EXPECT(2 == 2) failed on line"));
  EXPECT(Result.Message.contains("EXPECT_FALSE(1 == 1) failed on line"));
}

TEST(Assert_false_fails)
{
  assert_false_failure FailingTest;
  const auto Result = FailingTest.Run();
  ASSERT(Result.Message.contains("ASSERT_FALSE(true) failed on line"));
}

TEST(Assert_eq_reports_values)
{
  assert_eq FailingTest;
  const auto Result = FailingTest.Run();
  ASSERT_FALSE(Result.Message.contains("1 == 1"));
  const auto FirstFailure = R"(EXPECT_EQ(1, 2) failed on line 51.
  Lhs: 1
  Rhs: 2
)";

  const auto SecondFailure = R"(ASSERT_EQ(1, 2) failed on line 54.
  Lhs: 1
  Rhs: 2
)";
  ASSERT(Result.Message.contains(FirstFailure));
  ASSERT(Result.Message.contains(SecondFailure));
  ASSERT_FALSE(Result.Message.contains("ASSERT_EQ(2, 3)"));
}

TEST(Assert_ne_reports_values)
{
  assert_ne FailingTest;
  const auto Result = FailingTest.Run();
  const auto FirstFailure = R"(EXPECT_NE(1, 1) failed on line 66.
  Lhs: 1
  Rhs: 1
)";

  const auto SecondFailure = R"(ASSERT_NE(1, 1) failed on line 69.
  Lhs: 1
  Rhs: 1
)";
  ASSERT(Result.Message.contains(FirstFailure));
  ASSERT(Result.Message.contains(SecondFailure));
  ASSERT_FALSE(Result.Message.contains("ASSERT_NE(1, 2)"));
  ASSERT_FALSE(Result.Message.contains("EXPECT_NE(1, 2)"));
}

TEST(Assert_no_throw_reports_exception)
{
  class assert_no_throw : public test
  {
   public:
    constexpr assert_no_throw() : test("assert_no_throw", __FILE__) {}
    void RunImpl() override
    {
      ASSERT_NO_THROW([]() {}());
      ASSERT_NO_THROW([]() { throw std::bad_alloc(); }());
    }
  };

  assert_no_throw FailingTest;
  const auto Result = FailingTest.Run();
  std::cerr << Result.Message << "\n";
  ASSERT_FALSE(Result.Passed);
  ASSERT(Result.Message.contains("bad allocation"));
}

TEST(Assert_no_throw_reports_unknown_exception)
{
  class assert_no_throw_unknown : public test
  {
   public:
    constexpr assert_no_throw_unknown()
        : test("assert_no_throw_unknown", __FILE__)
    {
    }
    void RunImpl() override
    {
      ASSERT_NO_THROW([]() {}());
      ASSERT_NO_THROW([]() { throw 4; }());
    }
  };
  assert_no_throw_unknown FailingTest;
  const auto Result = FailingTest.Run();
  ASSERT_FALSE(Result.Passed);
  ASSERT(Result.Message.contains("Threw unknown exception"));
}

UNIT_TEST_INIT

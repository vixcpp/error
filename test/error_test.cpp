/**
 *
 *  @file error_test.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 */

#include <cstdlib>
#include <iostream>
#include <string>

#include <vix/error/Error.hpp>
#include <vix/error/ErrorCategory.hpp>
#include <vix/error/ErrorCode.hpp>
#include <vix/error/Exception.hpp>
#include <vix/error/Result.hpp>

namespace
{

  void assert_true(bool condition, const std::string &message)
  {
    if (!condition)
    {
      std::cerr << "[FAIL] " << message << '\n';
      std::exit(EXIT_FAILURE);
    }
  }

  void test_error_code_default_success()
  {
    vix::error::Error err;

    assert_true(err.code() == vix::error::ErrorCode::Ok,
                "Default Error should have code Ok.");
    assert_true(err.ok(),
                "Default Error should be ok.");
    assert_true(!err.has_error(),
                "Default Error should not have an error.");
    assert_true(!err,
                "Default Error boolean conversion should be false.");
  }

  void test_error_category_builtin_names()
  {
    using vix::error::ErrorCategory;

    assert_true(ErrorCategory::generic().name() == "generic",
                "generic category name should be 'generic'.");
    assert_true(ErrorCategory::system().name() == "system",
                "system category name should be 'system'.");
    assert_true(ErrorCategory::io().name() == "io",
                "io category name should be 'io'.");
    assert_true(ErrorCategory::network().name() == "network",
                "network category name should be 'network'.");
    assert_true(ErrorCategory::validation().name() == "validation",
                "validation category name should be 'validation'.");
  }

  void test_error_construction()
  {
    using vix::error::Error;
    using vix::error::ErrorCategory;
    using vix::error::ErrorCode;

    const Error err(
        ErrorCode::InvalidArgument,
        ErrorCategory::validation(),
        "invalid port");

    assert_true(err.code() == ErrorCode::InvalidArgument,
                "Error code should match constructor value.");
    assert_true(err.category() == ErrorCategory::validation(),
                "Error category should match constructor value.");
    assert_true(err.message() == "invalid port",
                "Error message should match constructor value.");
    assert_true(!err.ok(),
                "Constructed error should not be ok.");
    assert_true(err.has_error(),
                "Constructed error should report failure.");
    assert_true(static_cast<bool>(err),
                "Constructed error boolean conversion should be true.");
  }

  void test_error_equality()
  {
    using vix::error::Error;
    using vix::error::ErrorCategory;
    using vix::error::ErrorCode;

    const Error a(
        ErrorCode::IoError,
        ErrorCategory::io(),
        "failed to read file");

    const Error b(
        ErrorCode::IoError,
        ErrorCategory::io(),
        "failed to read file");

    const Error c(
        ErrorCode::NetworkError,
        ErrorCategory::network(),
        "connection lost");

    assert_true(a == b,
                "Equal errors should compare equal.");
    assert_true(a != c,
                "Different errors should compare not equal.");
  }

  void test_result_success()
  {
    vix::error::Result<int> result(42);

    assert_true(result.ok(),
                "Successful Result<int> should be ok.");
    assert_true(!result.has_error(),
                "Successful Result<int> should not contain an error.");
    assert_true(static_cast<bool>(result),
                "Successful Result<int> boolean conversion should be true.");
    assert_true(result.value() == 42,
                "Successful Result<int> should store the correct value.");
  }

  void test_result_failure()
  {
    using vix::error::Error;
    using vix::error::ErrorCategory;
    using vix::error::ErrorCode;
    using vix::error::Result;

    Result<int> result(
        Error(
            ErrorCode::InvalidArgument,
            ErrorCategory::validation(),
            "division by zero"));

    assert_true(!result.ok(),
                "Failed Result<int> should not be ok.");
    assert_true(result.has_error(),
                "Failed Result<int> should contain an error.");
    assert_true(!static_cast<bool>(result),
                "Failed Result<int> boolean conversion should be false.");
    assert_true(result.error().code() == ErrorCode::InvalidArgument,
                "Stored error code should match.");
    assert_true(result.error().category() == ErrorCategory::validation(),
                "Stored error category should match.");
    assert_true(result.error().message() == "division by zero",
                "Stored error message should match.");
  }

  void test_exception_wraps_error()
  {
    using vix::error::Error;
    using vix::error::ErrorCategory;
    using vix::error::ErrorCode;
    using vix::error::Exception;

    const Exception ex(
        Error(
            ErrorCode::IoError,
            ErrorCategory::io(),
            "failed to open file"));

    assert_true(std::string(ex.what()) == "failed to open file",
                "Exception what() should expose the wrapped error message.");
    assert_true(ex.error().code() == ErrorCode::IoError,
                "Exception should preserve error code.");
    assert_true(ex.error().category() == ErrorCategory::io(),
                "Exception should preserve error category.");
    assert_true(ex.error().message() == "failed to open file",
                "Exception should preserve error message.");
  }

} // namespace

int main()
{
  test_error_code_default_success();
  test_error_category_builtin_names();
  test_error_construction();
  test_error_equality();
  test_result_success();
  test_result_failure();
  test_exception_wraps_error();

  std::cout << "[PASS] vix_error_test\n";
  return EXIT_SUCCESS;
}

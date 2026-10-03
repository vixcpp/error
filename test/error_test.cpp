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
#include <type_traits>
#include <utility>
#include <variant>

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

  void test_result_rejects_success_error()
  {
    bool rejected = false;
    try
    {
      [[maybe_unused]] vix::error::Result<int> result(vix::error::Error{});
    }
    catch (const std::invalid_argument &)
    {
      rejected = true;
    }

    assert_true(rejected,
                "Result<int> should reject an Error that represents success.");
  }

  void test_result_invalid_access()
  {
    vix::error::Result<int> success(42);
    bool value_error_threw = false;
    try
    {
      [[maybe_unused]] const auto &error = success.error();
    }
    catch (const std::bad_variant_access &)
    {
      value_error_threw = true;
    }
    assert_true(value_error_threw,
                "error() on a successful Result should throw bad_variant_access.");

    vix::error::Result<int> failure(vix::error::Error(
        vix::error::ErrorCode::InvalidArgument,
        vix::error::ErrorCategory::validation(),
        "invalid value"));
    bool error_value_threw = false;
    try
    {
      [[maybe_unused]] const auto &value = failure.value();
    }
    catch (const std::bad_variant_access &)
    {
      error_value_threw = true;
    }
    assert_true(error_value_threw,
                "value() on a failed Result should throw bad_variant_access.");
  }

  void test_result_rvalue_access()
  {
    vix::error::Result<std::string> success(std::string("moved value"));
    const auto value = std::move(success).value();
    assert_true(value == "moved value",
                "rvalue value() should move the stored value out.");

    vix::error::Result<int> failure(vix::error::Error(
        vix::error::ErrorCode::IoError,
        vix::error::ErrorCategory::io(),
        "moved error"));
    const auto error = std::move(failure).error();
    assert_true(error.message() == "moved error",
                "rvalue error() should move the stored error out.");
  }

  void test_result_map_and_then()
  {
    using vix::error::Error;
    using vix::error::ErrorCategory;
    using vix::error::ErrorCode;
    using vix::error::Result;

    const Result<int> success(21);
    const auto mapped = success.map([](int value) { return value * 2; });
    assert_true(mapped.ok() && mapped.value() == 42,
                "map should transform a successful value.");

    const Result<int> failed(Error(
        ErrorCode::InvalidArgument,
        ErrorCategory::validation(),
        "invalid input"));
    const auto mapped_failure = failed.map([](int value) { return value * 2; });
    assert_true(mapped_failure.has_error() &&
                    mapped_failure.error().message() == "invalid input",
                "map should propagate a failure unchanged.");

    const auto chained = success.and_then([](int value) {
      return Result<std::string>(std::to_string(value));
    });
    assert_true(chained.ok() && chained.value() == "21",
                "and_then should return the operation result for success.");

    const auto chained_failure = failed.and_then([](int value) {
      return Result<std::string>(std::to_string(value));
    });
    assert_true(chained_failure.has_error() &&
                    chained_failure.error().message() == "invalid input",
                "and_then should propagate a failure unchanged.");
  }

  void test_result_supported_type_contract()
  {
    static_assert(std::is_same_v<vix::error::Result<int>::value_type, int>);
    static_assert(std::is_same_v<vix::error::Result<int>::error_type,
                                 vix::error::Error>);
    static_assert(!std::is_default_constructible_v<vix::error::Result<int>>);
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
  test_result_rejects_success_error();
  test_result_invalid_access();
  test_result_rvalue_access();
  test_result_map_and_then();
  test_result_supported_type_contract();
  test_exception_wraps_error();

  std::cout << "[PASS] vix_error_test\n";
  return EXIT_SUCCESS;
}

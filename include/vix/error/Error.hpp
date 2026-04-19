/**
 *
 *  @file Error.hpp
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
#ifndef VIX_ERROR_ERROR_HPP
#define VIX_ERROR_ERROR_HPP

#include <string>
#include <string_view>
#include <utility>

#include <vix/error/ErrorCategory.hpp>
#include <vix/error/ErrorCode.hpp>

namespace vix::error
{

  /**
   * @class Error
   * @brief Structured error object used across Vix.
   *
   * Error combines:
   * - an ErrorCode describing what went wrong
   * - an ErrorCategory describing the domain of the failure
   * - a human-readable message for logs, diagnostics, and debugging
   *
   * Design goals:
   * - simple and explicit
   * - easy to construct and return
   * - easy to inspect in logs and APIs
   * - stable enough to be reused across all Vix modules
   *
   * Example:
   * @code
   * return Error(
   *   ErrorCode::InvalidArgument,
   *   ErrorCategory::validation(),
   *   "port must be greater than 0"
   * );
   * @endcode
   *
   * Notes:
   * - default-constructed Error represents success (`ErrorCode::Ok`)
   * - message is owned by the Error object
   */
  class Error
  {
  public:
    /**
     * @brief Construct a success error object.
     *
     * A default-constructed Error means "no error".
     */
    Error() = default;

    /**
     * @brief Construct an error with code only.
     *
     * Category defaults to `generic()`.
     *
     * @param code Error code.
     */
    explicit Error(ErrorCode code) noexcept
        : code_(code)
    {
    }

    /**
     * @brief Construct an error with code and message.
     *
     * Category defaults to `generic()`.
     *
     * @param code Error code.
     * @param message Human-readable error message.
     */
    Error(ErrorCode code, std::string message)
        : code_(code),
          message_(std::move(message))
    {
    }

    /**
     * @brief Construct an error with code, category, and message.
     *
     * @param code Error code.
     * @param category Error category.
     * @param message Human-readable error message.
     */
    Error(ErrorCode code, ErrorCategory category, std::string message)
        : code_(code),
          category_(category),
          message_(std::move(message))
    {
    }

    /**
     * @brief Get the error code.
     *
     * @return The stored ErrorCode.
     */
    [[nodiscard]] constexpr ErrorCode code() const noexcept
    {
      return code_;
    }

    /**
     * @brief Get the error category.
     *
     * @return The stored ErrorCategory.
     */
    [[nodiscard]] constexpr ErrorCategory category() const noexcept
    {
      return category_;
    }

    /**
     * @brief Get the human-readable error message.
     *
     * @return A string view over the stored message.
     */
    [[nodiscard]] std::string_view message() const noexcept
    {
      return message_;
    }

    /**
     * @brief Get the human-readable error message as a C string.
     *
     * This is useful for interoperability with APIs such as
     * std::exception::what().
     *
     * @return Pointer to a null-terminated message string.
     */
    [[nodiscard]] const char *message_c_str() const noexcept
    {
      return message_.c_str();
    }

    /**
     * @brief Returns true if this object represents success.
     *
     * @return true if code() == ErrorCode::Ok, false otherwise.
     */
    [[nodiscard]] constexpr bool ok() const noexcept
    {
      return code_ == ErrorCode::Ok;
    }

    /**
     * @brief Returns true if this object represents a failure.
     *
     * @return true if this object contains an error, false otherwise.
     */
    [[nodiscard]] constexpr bool has_error() const noexcept
    {
      return !ok();
    }

    /**
     * @brief Boolean conversion for convenience.
     *
     * Returns true when the object contains an error.
     *
     * Example:
     * @code
     * if (err)
     * {
     *   // handle error
     * }
     * @endcode
     *
     * @return true if this object represents a failure.
     */
    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return has_error();
    }

    /**
     * @brief Compare two Error objects for equality.
     *
     * @param other Error to compare with.
     * @return true if code, category, and message are equal.
     */
    [[nodiscard]] bool operator==(const Error &other) const noexcept
    {
      return code_ == other.code_ &&
             category_ == other.category_ &&
             message_ == other.message_;
    }

    /**
     * @brief Compare two Error objects for inequality.
     *
     * @param other Error to compare with.
     * @return true if the two errors differ.
     */
    [[nodiscard]] bool operator!=(const Error &other) const noexcept
    {
      return !(*this == other);
    }

  private:
    ErrorCode code_{ErrorCode::Ok};
    ErrorCategory category_{ErrorCategory::generic()};
    std::string message_{};
  };

} // namespace vix::error

#endif // VIX_ERROR_ERROR_HPP

/**
 *
 *  @file Exception.hpp
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
#ifndef VIX_ERROR_EXCEPTION_HPP
#define VIX_ERROR_EXCEPTION_HPP

#include <exception>
#include <string>
#include <utility>

#include <vix/error/Error.hpp>

namespace vix::error
{

  /**
   * @class Exception
   * @brief Standard exception wrapper around vix::error::Error.
   *
   * Exception provides optional interoperability with exception-based C++ code.
   * It wraps a structured Error object while still exposing a standard
   * `what()` message through `std::exception`.
   *
   * Design goals:
   * - keep Error as the primary abstraction
   * - provide a clean bridge for throw/catch code paths
   * - preserve structured error information
   *
   * Example:
   * @code
   * throw Exception(
   *   Error(
   *     ErrorCode::IoError,
   *     ErrorCategory::io(),
   *     "failed to open file"
   *   )
   * );
   * @endcode
   */
  class Exception : public std::exception
  {
  public:
    /**
     * @brief Construct from an Error object.
     *
     * @param error Structured error to wrap.
     */
    explicit Exception(Error error) noexcept
        : error_(std::move(error))
    {
    }

    /**
     * @brief Construct directly from code and message.
     *
     * Category defaults to generic().
     *
     * @param code Error code.
     * @param message Human-readable message.
     */
    Exception(ErrorCode code, std::string message) noexcept
        : error_(code, std::move(message))
    {
    }

    /**
     * @brief Construct directly from code, category, and message.
     *
     * @param code Error code.
     * @param category Error category.
     * @param message Human-readable message.
     */
    Exception(ErrorCode code, ErrorCategory category, std::string message) noexcept
        : error_(code, category, std::move(message))
    {
    }

    /**
     * @brief Return the exception message.
     */
    [[nodiscard]] const char *what() const noexcept override
    {
      return error_.message_c_str();
    }

    /**
     * @brief Access the wrapped Error object.
     */
    [[nodiscard]] const Error &error() const noexcept
    {
      return error_;
    }

  private:
    Error error_;
  };

} // namespace vix::error

#endif // VIX_ERROR_EXCEPTION_HPP

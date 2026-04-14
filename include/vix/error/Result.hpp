/**
 *
 *  @file Result.hpp
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
#ifndef VIX_ERROR_RESULT_HPP
#define VIX_ERROR_RESULT_HPP

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include <vix/error/Error.hpp>

namespace vix::error
{

  /**
   * @class Result
   * @brief Represents either a success value of type T or an Error.
   *
   * Result<T> is the preferred explicit error-handling abstraction in Vix.
   * It avoids forcing exceptions while keeping APIs clear and expressive.
   *
   * A Result<T> contains exactly one of:
   * - a value of type T
   * - an Error describing the failure
   *
   * Beginner-friendly usage:
   * @code
   * Result<int> divide(int a, int b)
   * {
   *   if (b == 0)
   *   {
   *     return Error(
   *       ErrorCode::InvalidArgument,
   *       ErrorCategory::validation(),
   *       "division by zero");
   *   }
   *
   *   return a / b;
   * }
   * @endcode
   *
   * Expert-friendly usage:
   * - Compose operations with `and_then`
   * - Transform values with `map`
   * - Avoid exceptions in performance-critical paths
   *
   * @tparam T Success value type.
   */
  template <typename T>
  class Result
  {
    static_assert(!std::is_reference_v<T>,
                  "Result<T> does not support reference types.");
    static_assert(!std::is_same_v<std::remove_cv_t<T>, Error>,
                  "Result<Error> is not allowed. Use Error directly.");
    static_assert(!std::is_void_v<T>,
                  "Result<void> requires specialization.");

  public:
    /// @brief Type of the success value.
    using value_type = T;

    /// @brief Type of the error.
    using error_type = Error;

    /**
     * @brief Construct a successful result.
     */
    Result(const T &value)
        : storage_(value)
    {
    }

    /**
     * @brief Construct a successful result (move).
     */
    Result(T &&value)
        : storage_(std::move(value))
    {
    }

    /**
     * @brief Construct a failed result.
     *
     * @throws std::invalid_argument if error represents success.
     */
    Result(const Error &error)
        : storage_(validate_error(error))
    {
    }

    /**
     * @brief Construct a failed result (move).
     *
     * @throws std::invalid_argument if error represents success.
     */
    Result(Error &&error)
        : storage_(validate_error(std::move(error)))
    {
    }

    /**
     * @brief Returns true if the result contains a value.
     */
    [[nodiscard]] bool ok() const noexcept
    {
      return std::holds_alternative<T>(storage_);
    }

    /**
     * @brief Returns true if the result contains an error.
     */
    [[nodiscard]] bool has_error() const noexcept
    {
      return !ok();
    }

    /**
     * @brief Boolean conversion.
     *
     * True when success.
     */
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return ok();
    }

    /**
     * @brief Access the success value.
     *
     * @warning Must be called only if ok() is true.
     */
    [[nodiscard]] T &value() &
    {
      return std::get<T>(storage_);
    }

    /**
     * @brief Access the success value (const).
     */
    [[nodiscard]] const T &value() const &
    {
      return std::get<T>(storage_);
    }

    /**
     * @brief Move the success value.
     */
    [[nodiscard]] T &&value() &&
    {
      return std::get<T>(std::move(storage_));
    }

    /**
     * @brief Access the error.
     *
     * @warning Must be called only if has_error() is true.
     */
    [[nodiscard]] Error &error() &
    {
      return std::get<Error>(storage_);
    }

    /**
     * @brief Access the error (const).
     */
    [[nodiscard]] const Error &error() const &
    {
      return std::get<Error>(storage_);
    }

    /**
     * @brief Move the error.
     */
    [[nodiscard]] Error &&error() &&
    {
      return std::get<Error>(std::move(storage_));
    }

    /**
     * @brief Transform the success value.
     *
     * Applies fn only if success.
     */
    template <typename F>
    [[nodiscard]] auto map(F &&fn) const
        -> Result<std::invoke_result_t<F, T>>
    {
      using U = std::invoke_result_t<F, T>;

      if (ok())
      {
        return Result<U>(fn(std::get<T>(storage_)));
      }

      return std::get<Error>(storage_);
    }

    /**
     * @brief Chain operations returning Result.
     *
     * Applies fn only if success.
     */
    template <typename F>
    [[nodiscard]] auto and_then(F &&fn) const
        -> decltype(fn(std::declval<T>()))
    {
      if (ok())
      {
        return fn(std::get<T>(storage_));
      }

      return decltype(fn(std::declval<T>()))(std::get<Error>(storage_));
    }

  private:
    /**
     * @brief Validate that error represents a failure.
     */
    [[nodiscard]] static Error validate_error(const Error &error)
    {
      if (!error.has_error())
      {
        throw std::invalid_argument(
            "Result<T> cannot contain a success Error.");
      }
      return error;
    }

    /**
     * @brief Validate that error represents a failure (move).
     */
    [[nodiscard]] static Error validate_error(Error &&error)
    {
      if (!error.has_error())
      {
        throw std::invalid_argument(
            "Result<T> cannot contain a success Error.");
      }
      return std::move(error);
    }

  private:
    std::variant<T, Error> storage_;
  };

} // namespace vix::error

#endif // VIX_ERROR_RESULT_HPP

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
   * It allows APIs to return either:
   * - a success value of type T
   * - a failure represented by vix::error::Error
   *
   * This avoids forcing exception-based control flow while keeping APIs
   * clear, explicit, and easy to compose.
   *
   * A Result<T> always contains exactly one of:
   * - a value
   * - an error
   *
   * Typical usage:
   * @code
   * Result<int> divide(int a, int b)
   * {
   *   if (b == 0)
   *   {
   *     return Error(
   *       ErrorCode::InvalidArgument,
   *       ErrorCategory::validation(),
   *       "division by zero"
   *     );
   *   }
   *
   *   return a / b;
   * }
   * @endcode
   *
   * Composition example:
   * @code
   * auto result = divide(10, 2)
   *   .map([](int x)
   *   {
   *     return x * 2;
   *   });
   * @endcode
   *
   * @tparam T Type of the success value.
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
    /**
     * @brief Success value type.
     */
    using value_type = T;

    /**
     * @brief Error type.
     */
    using error_type = Error;

    /**
     * @brief Construct a successful result from a const value.
     *
     * @param value Success value to store.
     */
    Result(const T &value)
        : storage_(value)
    {
    }

    /**
     * @brief Construct a successful result from an rvalue.
     *
     * @param value Success value to move into the result.
     */
    Result(T &&value)
        : storage_(std::move(value))
    {
    }

    /**
     * @brief Construct a failed result from a const Error.
     *
     * @param error Error to store.
     *
     * @throws std::invalid_argument if the provided Error represents success.
     */
    Result(const Error &error)
        : storage_(validate_error(error))
    {
    }

    /**
     * @brief Construct a failed result from an rvalue Error.
     *
     * @param error Error to move into the result.
     *
     * @throws std::invalid_argument if the provided Error represents success.
     */
    Result(Error &&error)
        : storage_(validate_error(std::move(error)))
    {
    }

    /**
     * @brief Returns true if the result contains a success value.
     *
     * @return true if a value is stored, false otherwise.
     */
    [[nodiscard]] bool ok() const noexcept
    {
      return std::holds_alternative<T>(storage_);
    }

    /**
     * @brief Returns true if the result contains an error.
     *
     * @return true if an error is stored, false otherwise.
     */
    [[nodiscard]] bool has_error() const noexcept
    {
      return !ok();
    }

    /**
     * @brief Boolean conversion operator.
     *
     * Equivalent to ok().
     *
     * @return true if the result contains a value.
     */
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return ok();
    }

    /**
     * @brief Access the stored success value.
     *
     * @warning Must only be called when ok() is true.
     *
     * @return Reference to the stored value.
     */
    [[nodiscard]] T &value() &
    {
      return std::get<T>(storage_);
    }

    /**
     * @brief Access the stored success value.
     *
     * @warning Must only be called when ok() is true.
     *
     * @return Const reference to the stored value.
     */
    [[nodiscard]] const T &value() const &
    {
      return std::get<T>(storage_);
    }

    /**
     * @brief Move the stored success value out of the result.
     *
     * @warning Must only be called when ok() is true.
     *
     * @return Rvalue reference to the stored value.
     */
    [[nodiscard]] T &&value() &&
    {
      return std::get<T>(std::move(storage_));
    }

    /**
     * @brief Access the stored error.
     *
     * @warning Must only be called when has_error() is true.
     *
     * @return Reference to the stored error.
     */
    [[nodiscard]] Error &error() &
    {
      return std::get<Error>(storage_);
    }

    /**
     * @brief Access the stored error.
     *
     * @warning Must only be called when has_error() is true.
     *
     * @return Const reference to the stored error.
     */
    [[nodiscard]] const Error &error() const &
    {
      return std::get<Error>(storage_);
    }

    /**
     * @brief Move the stored error out of the result.
     *
     * @warning Must only be called when has_error() is true.
     *
     * @return Rvalue reference to the stored error.
     */
    [[nodiscard]] Error &&error() &&
    {
      return std::get<Error>(std::move(storage_));
    }

    /**
     * @brief Transform the success value while preserving error propagation.
     *
     * If this result contains a value, fn is applied to that value and a new
     * Result<U> is returned.
     *
     * If this result contains an error, the same error is propagated.
     *
     * @tparam F Callable type.
     * @param fn Transformation function applied to the stored value.
     *
     * @return A Result containing either the transformed value or the original error.
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
     * @brief Chain another operation that itself returns a Result.
     *
     * If this result contains a value, fn is applied to that value.
     *
     * If this result contains an error, the same error is propagated into the
     * returned Result type.
     *
     * @tparam F Callable type.
     * @param fn Function returning another Result-like object.
     *
     * @return The result produced by fn, or the propagated error.
     */
    template <typename F>
    [[nodiscard]] auto and_then(F &&fn) const
        -> decltype(fn(std::declval<T>()))
    {
      using ReturnType = decltype(fn(std::declval<T>()));

      if (ok())
      {
        return fn(std::get<T>(storage_));
      }

      return ReturnType(std::get<Error>(storage_));
    }

  private:
    /**
     * @brief Validate that an Error actually represents a failure.
     *
     * Result<T> cannot store a success-state Error.
     *
     * @param error Error to validate.
     *
     * @return A validated Error.
     *
     * @throws std::invalid_argument if error does not represent a failure.
     */
    static Error validate_error(const Error &error)
    {
      if (!error.has_error())
      {
        throw std::invalid_argument(
            "Result<T> cannot contain a success Error.");
      }

      return error;
    }

    /**
     * @brief Validate that an Error actually represents a failure.
     *
     * Result<T> cannot store a success-state Error.
     *
     * @param error Error to validate and move.
     *
     * @return A validated Error.
     *
     * @throws std::invalid_argument if error does not represent a failure.
     */
    static Error validate_error(Error &&error)
    {
      if (!error.has_error())
      {
        throw std::invalid_argument(
            "Result<T> cannot contain a success Error.");
      }

      return std::move(error);
    }

  private:
    /**
     * @brief Internal storage for either the success value or the error.
     */
    std::variant<T, Error> storage_;
  };

} // namespace vix::error

#endif // VIX_ERROR_RESULT_HPP

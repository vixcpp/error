/**
 *
 *  @file ErrorCategory.hpp
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
#ifndef VIX_ERROR_ERRORCATEGORY_HPP
#define VIX_ERROR_ERRORCATEGORY_HPP

#include <string_view>

namespace vix::error
{

  /**
   * @class ErrorCategory
   * @brief High-level classification of errors.
   *
   * ErrorCategory provides a coarse-grained grouping of errors.
   * While ErrorCode describes *what* went wrong, ErrorCategory
   * describes *where* or *in which domain* the error belongs.
   *
   * Design goals:
   * - simple and lightweight (no dynamic allocation)
   * - usable in logs, debugging, and monitoring
   * - extensible without breaking existing code
   *
   * Example:
   * @code
   * ErrorCategory::network()
   * ErrorCategory::io()
   * ErrorCategory::system()
   * @endcode
   */
  class ErrorCategory
  {
  public:
    /**
     * @brief Construct a category from a name.
     *
     * @param name Static category name (must outlive the program).
     */
    constexpr explicit ErrorCategory(std::string_view name) noexcept
        : name_(name)
    {
    }

    /**
     * @brief Get category name.
     */
    [[nodiscard]] constexpr std::string_view name() const noexcept
    {
      return name_;
    }

    /**
     * @brief Compare two categories.
     */
    [[nodiscard]] constexpr bool operator==(const ErrorCategory &other) const noexcept
    {
      return name_ == other.name_;
    }

    /**
     * @brief Compare two categories.
     */
    [[nodiscard]] constexpr bool operator!=(const ErrorCategory &other) const noexcept
    {
      return !(*this == other);
    }

    // ------------------------------------------------------------------
    // Built-in categories
    // ------------------------------------------------------------------

    /**
     * @brief Generic / unknown category.
     */
    static constexpr ErrorCategory generic() noexcept
    {
      return ErrorCategory("generic");
    }

    /**
     * @brief System-level errors (OS, signals, low-level failures).
     */
    static constexpr ErrorCategory system() noexcept
    {
      return ErrorCategory("system");
    }

    /**
     * @brief I/O and filesystem errors.
     */
    static constexpr ErrorCategory io() noexcept
    {
      return ErrorCategory("io");
    }

    /**
     * @brief Network-related errors.
     */
    static constexpr ErrorCategory network() noexcept
    {
      return ErrorCategory("network");
    }

    /**
     * @brief Validation-related errors.
     */
    static constexpr ErrorCategory validation() noexcept
    {
      return ErrorCategory("validation");
    }

    /**
     * @brief Configuration-related errors.
     */
    static constexpr ErrorCategory config() noexcept
    {
      return ErrorCategory("config");
    }

    /**
     * @brief Security-related errors (auth, permissions, crypto).
     */
    static constexpr ErrorCategory security() noexcept
    {
      return ErrorCategory("security");
    }

    /**
     * @brief Internal runtime errors.
     */
    static constexpr ErrorCategory internal() noexcept
    {
      return ErrorCategory("internal");
    }

  private:
    std::string_view name_;
  };

} // namespace vix::error

#endif // VIX_ERROR_ERRORCATEGORY_HPP

/**
 *
 *  @file ErrorCode.hpp
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
#ifndef VIX_ERROR_ERRORCODE_HPP
#define VIX_ERROR_ERRORCODE_HPP

#include <cstdint>

namespace vix::error
{

  /**
   * @enum ErrorCode
   * @brief Canonical error codes used across Vix modules.
   *
   * ErrorCode provides a small, stable, and explicit vocabulary
   * for reporting failures in a consistent way across the Vix ecosystem.
   *
   * Design goals:
   * - easy to understand
   * - easy to serialize/log
   * - stable enough to be shared across modules
   * - generic enough for fs, io, http, process, crypto, and runtime code
   *
   * Typical usage:
   * @code
   * return Error(ErrorCode::InvalidArgument, "port must be greater than 0");
   * @endcode
   *
   * Notes:
   * - `Ok` means success / no error.
   * - module-specific errors can still reuse these generic categories.
   * - keep this enum compact and stable over time.
   */
  enum class ErrorCode : std::uint32_t
  {
    /**
     * @brief No error.
     */
    Ok = 0,

    /**
     * @brief Unknown or unspecified error.
     */
    Unknown,

    /**
     * @brief Invalid function argument or invalid input value.
     */
    InvalidArgument,

    /**
     * @brief Operation is not valid in the current state.
     */
    InvalidState,

    /**
     * @brief A required value or object was missing.
     */
    NotFound,

    /**
     * @brief Resource already exists.
     */
    AlreadyExists,

    /**
     * @brief Operation timed out.
     */
    Timeout,

    /**
     * @brief Operation was cancelled.
     */
    Cancelled,

    /**
     * @brief Permission denied.
     */
    PermissionDenied,

    /**
     * @brief Authentication failed.
     */
    Unauthorized,

    /**
     * @brief Access was refused or forbidden.
     */
    Forbidden,

    /**
     * @brief Requested operation is not supported.
     */
    NotSupported,

    /**
     * @brief A resource limit was reached.
     */
    ResourceExhausted,

    /**
     * @brief Memory allocation or memory-related failure.
     */
    OutOfMemory,

    /**
     * @brief I/O operation failed.
     */
    IoError,

    /**
     * @brief Filesystem-related failure.
     */
    FilesystemError,

    /**
     * @brief Network-related failure.
     */
    NetworkError,

    /**
     * @brief Protocol parsing or protocol contract failure.
     */
    ProtocolError,

    /**
     * @brief Data format or serialization/deserialization failure.
     */
    ParseError,

    /**
     * @brief Validation failed.
     */
    ValidationError,

    /**
     * @brief Configuration is invalid or incomplete.
     */
    ConfigError,

    /**
     * @brief Internal invariant was broken or unexpected internal failure occurred.
     */
    InternalError,

    /**
     * @brief Feature is declared but not implemented yet.
     */
    NotImplemented,

    /**
     * @brief External dependency or third-party system failed.
     */
    ExternalError
  };

} // namespace vix::error

#endif // VIX_ERROR_ERRORCODE_HPP

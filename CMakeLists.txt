#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira. All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - Error Module
# ====================================================================
# Purpose:
#   Unified error handling primitives for Vix.
#   Provides structured error codes, categories, error objects,
#   result-based APIs, and optional exception interop.
#
#   Builds as STATIC when sources exist, otherwise as a header-only
#   INTERFACE target.
#
# Public Targets:
#   - vix_error  : The actual library target (STATIC or INTERFACE)
#   - vix::error : Namespaced alias for consumers
#
# Public Dependencies:
#   - (none by default)
#
# Installation/Export:
#   - Umbrella build exports into: VixTargets
#   - Standalone build exports into: vix_errorTargets
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_error VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_ERROR_EXPORT_SET VixTargets)
else()
  set(VIX_ERROR_EXPORT_SET vix_errorTargets)
endif()

# --------------------------------------------------------------------
# Sources discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE ERROR_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------------------------
# Library target
# --------------------------------------------------------------------
if (ERROR_SOURCES)
  message(STATUS "[error] Building STATIC library with detected sources.")

  add_library(vix_error STATIC ${ERROR_SOURCES})
  add_library(vix::error ALIAS vix_error)

  target_compile_features(vix_error PUBLIC cxx_std_20)

  target_include_directories(vix_error
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # Build-only helpers must not leak into exported interfaces.
  if (NOT MSVC)
    target_compile_options(vix_error PRIVATE
      -Wall
      -Wextra
      -Wpedantic
    )
  endif()

  if (VIX_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(vix_error PRIVATE
      -fno-omit-frame-pointer
      -fsanitize=address,undefined
    )
    target_link_options(vix_error PRIVATE
      -fsanitize=address,undefined
    )
  endif()

  set_target_properties(vix_error PROPERTIES
    OUTPUT_NAME vix_error
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME error
  )

  install(TARGETS vix_error
    EXPORT ${VIX_ERROR_EXPORT_SET}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

else()
  message(STATUS "[error] Building HEADER-ONLY library (no sources).")

  add_library(vix_error INTERFACE)
  add_library(vix::error ALIAS vix_error)

  target_compile_features(vix_error INTERFACE cxx_std_20)

  target_include_directories(vix_error INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  install(TARGETS vix_error
    EXPORT ${VIX_ERROR_EXPORT_SET}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# --------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
      PATTERN "*.hpp"
      PATTERN "*.h"
  )
endif()

# --------------------------------------------------------------------
# Standalone package config/export
# Only for non-umbrella builds
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vix_errorConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_errorConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_error"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/vix_errorConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/vix_errorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_errorConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_error"
  )

  install(EXPORT vix_errorTargets
    FILE vix_errorTargets.cmake
    NAMESPACE vix::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_error"
  )

endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
option(VIX_ERROR_BUILD_TESTS "Build error module tests" OFF)

if (VIX_ERROR_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "vix::error configured (${PROJECT_VERSION})")
if (ERROR_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Export set: ${VIX_ERROR_EXPORT_SET}")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/error/...>)")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")

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
#   Installs into the umbrella export-set `VixTargets`.
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_error VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Umbrella build policy:
# When building inside the Vix umbrella, dependencies are managed by the
# umbrella (add_subdirectory order + options). Error must remain fully
# standalone and must not FetchContent nor add sibling subdirectories.
# --------------------------------------------------------------------

# Sources discovery
file(GLOB_RECURSE ERROR_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# STATIC
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

  if (TARGET vix_warnings)
    target_link_libraries(vix_error PUBLIC vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_error PUBLIC vix_sanitizers)
  endif()

  set_target_properties(vix_error PROPERTIES
    OUTPUT_NAME vix_error
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME error
  )

  install(TARGETS vix_error
    EXPORT VixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

# HEADER-ONLY
else()
  message(STATUS "[error] Building HEADER-ONLY library (no sources).")

  add_library(vix_error INTERFACE)
  add_library(vix::error ALIAS vix_error)

  target_compile_features(vix_error INTERFACE cxx_std_20)

  target_include_directories(vix_error INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_error INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_error
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

endif()

# Install headers
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
endif()

# Package config
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

# Export targets
install(EXPORT VixTargets
  FILE VixTargets.cmake
  NAMESPACE vix::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_error"
)

# Tests
option(VIX_ERROR_BUILD_TESTS "Build error module tests" OFF)

if (VIX_ERROR_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

# Summary
message(STATUS "------------------------------------------------------")
message(STATUS "vix::error configured (${PROJECT_VERSION})")
if (ERROR_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/error/...>)")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")

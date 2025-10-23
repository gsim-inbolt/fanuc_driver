# SPDX-FileCopyrightText: 2025, FANUC America Corporation
# SPDX-FileCopyrightText: 2025, FANUC CORPORATION
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22)
project(fanuc_libs VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_STREAM_MOTION ON "Build stream_motion library")
option(BUILD_RMI ON "Build rmi library")
option(BUILD_FANUC_CLIENT ON "Build fanuc_client library")
option(BUILD_GPIO_CONFIG ON "Build gpio_config library")

# Build the sockpp library
include(FetchContent)
FetchContent_Declare(
    sockpp
    GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/dependencies/sockpp
    GIT_TAG afdeacba9448c7a77194eed6ab8e1c0b1653c79a
    PATCH_COMMAND bash ${CMAKE_SOURCE_DIR}/dependencies/apply_patch.bash ${CMAKE_SOURCE_DIR}/dependencies/sockpp.patch
)
set(SOCKPP_BUILD_SHARED OFF CACHE BOOL "Build sockpp as a shared library")
set(SOCKPP_BUILD_STATIC ON CACHE BOOL "Build sockpp as a static library")
FetchContent_MakeAvailable(sockpp)
set_target_properties(sockpp-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Create readerwriterqueue target
FetchContent_Declare(
    readerwriterqueue
    GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/dependencies/readerwriterqueue
    GIT_TAG 211616e0554f93152ab3108b8d93fbc23174a9d9
    PATCH_COMMAND bash ${CMAKE_SOURCE_DIR}/dependencies/apply_patch.bash ${CMAKE_SOURCE_DIR}/dependencies/readerwriterqueue.patch
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/dependencies/yaml-cpp
  GIT_TAG 2f86d13775d119edbb69af52e5f566fd65c6953b
)
FetchContent_MakeAvailable(yaml-cpp)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp shared library")
set(YAML_CPP_INSTALL OFF CACHE BOOL "Enable generation of yaml-cpp install targets")

# Build the reflectcpp library
FetchContent_Declare(
    reflect-cpp
    GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/dependencies/reflect-cpp
    GIT_TAG 2a0fdeabe35bdab0e07e05d4f869ca88bd6d2b8c
    PATCH_COMMAND bash ${CMAKE_SOURCE_DIR}/dependencies/apply_patch.bash ${CMAKE_SOURCE_DIR}/dependencies/reflectcpp.patch
)
set(REFLECTCPP_BUILD_SHARED OFF CACHE BOOL "Build reflectcpp as a shared library")
set(REFLECTCPP_YAML ON CACHE BOOL "Enable YAML support")
FetchContent_MakeAvailable(reflect-cpp)
set_target_properties(reflectcpp PROPERTIES CXX_STANDARD 20)
set_target_properties(reflectcpp PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (BUILD_STREAM_MOTION)
  # Build the fmt library
  FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt.git
      GIT_TAG 12.0.0
  )
  FetchContent_MakeAvailable(fmt)
endif()

if (BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest." FORCE)
  set(BUILD_GMOCK ON CACHE BOOL "Builds the googlemock subproject." FORCE)
  FetchContent_MakeAvailable(googletest)
  include(GoogleTest)
  enable_testing()
endif ()

if (BUILD_STREAM_MOTION)
  add_subdirectory(stream_motion)
  add_subdirectory(examples_stmo)
endif()
if (BUILD_RMI)
  add_subdirectory(rmi)
endif()
if (BUILD_FANUC_CLIENT)
  add_subdirectory(fanuc_client)
endif()
if (BUILD_GPIO_CONFIG)
  add_subdirectory(gpio_config)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()

# Create cmake config and version files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/fanuc_libsConfigVersion.cmake
    VERSION
    ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)
configure_package_config_file(fanuc_libsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/fanuc_libsConfig.cmake
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/fanuc_libs/
)

# --- Install ---
include(GNUInstallDirs)
set(INSTALL_TARGETS "")
if (BUILD_STREAM_MOTION)
  list(APPEND INSTALL_TARGETS stream_motion)
endif()
if (BUILD_RMI)
  list(APPEND INSTALL_TARGETS rmi)
endif()
if (BUILD_FANUC_CLIENT)
  list(APPEND INSTALL_TARGETS fanuc_client)
endif()
if (BUILD_GPIO_CONFIG)
  list(APPEND INSTALL_TARGETS gpio_config)
endif()
install(TARGETS ${INSTALL_TARGETS}
    EXPORT fanuc_libsExports
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if (INSTALL_TARGETS)
  install(EXPORT fanuc_libsExports
      FILE
      fanuc_libsTargets.cmake
      NAMESPACE
      fanuc_libs::
      DESTINATION
      ${CMAKE_INSTALL_LIBDIR}/cmake/fanuc_libs
  )
endif()
if (BUILD_STREAM_MOTION)
  install(DIRECTORY stream_motion/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
if (BUILD_RMI)
  install(DIRECTORY rmi/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
if (BUILD_FANUC_CLIENT)
  install(DIRECTORY fanuc_client/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
if (BUILD_GPIO_CONFIG)
  install(DIRECTORY gpio_config/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/fanuc_libsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/fanuc_libsConfigVersion.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/fanuc_libs
)

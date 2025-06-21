# -----------------------
# Fetch GoogleTest
# -----------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
if(NOT googletest_POPULATED)
  message(STATUS "🔽 Downloading GoogleTest from GitHub...")
endif()
FetchContent_MakeAvailable(googletest)

# -----------------------
# Enable testing system
# -----------------------
include(CTest)
enable_testing()

# -----------------------
# Add test executable
# -----------------------
add_executable(NoiseGeneratorTests
  ${CMAKE_SOURCE_DIR}/tests/test_noise_math.cpp
  ${CMAKE_SOURCE_DIR}/tests/test_noise_generator.cpp
)

target_include_directories(NoiseGeneratorTests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(NoiseGeneratorTests PRIVATE
  gtest
  gtest_main
)

target_sources(NoiseGeneratorTests PRIVATE
  ${CMAKE_SOURCE_DIR}/src/Noise/NoiseMath.cpp
  ${CMAKE_SOURCE_DIR}/src/Noise/NoiseGenerator.cpp
  ${CMAKE_SOURCE_DIR}/src/Noise/NoiseGenerator.h
  ${CMAKE_SOURCE_DIR}/src/Noise/NoiseTypes.h

  ${CMAKE_SOURCE_DIR}/src/Logger/Logger.cpp
  ${CMAKE_SOURCE_DIR}/src/Logger/Logger.h
  
)

set_target_properties(NoiseGeneratorTests PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

# -----------------------
# Register tests for CTest
# -----------------------
include(GoogleTest)
gtest_discover_tests(NoiseGeneratorTests)

# -----------------------
# Visual Studio startup project
# -----------------------
if(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NoiseGeneratorTests)
endif()
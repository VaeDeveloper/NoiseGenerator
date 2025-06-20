cmake_minimum_required(VERSION 3.11...3.27)
option(USE_UNICODE_MESSAGES "Use emoji in build messages" ON)

message(STATUS "🧾 Supported configs: ${CMAKE_CONFIGURATION_TYPES}")
# -----------------------
# Add Custom Configuration
# -----------------------
if(BUILD_TESTING_ONLY)
  set(CMAKE_CONFIGURATION_TYPES "Testing" CACHE STRING "" FORCE)
elseif(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)
endif()

# -----------------------
# MSVC Runtime Setup
# -----------------------
if(MSVC)
  if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "🧱 [CMake] MSVC detected — multi-config generator (Visual Studio)")
    if(CMAKE_CFG_INTDIR STREQUAL "Testing")
      message(STATUS "🧪 MSVC 'Testing' config detected — applying /MDd and test flags")
      set(CMAKE_CXX_FLAGS_TESTING "/MDd /DTESTING_MODE" CACHE STRING "" FORCE)
    endif()
  else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)
      message(STATUS "🧱 [CMake] Runtime: /MDd (Debug)")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Testing")
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)
      message(STATUS "🧱 [CMake] Runtime: /MDd (Testing)")
    else()
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "" FORCE)
      message(STATUS "🧱 [CMake] Runtime: /MD (Release or others)")
    endif()
  endif()

  if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    message(WARNING "⚠️ CMAKE_MSVC_RUNTIME_LIBRARY is not set explicitly — MSVC will choose default (/MD or /MDd)")
  endif()
endif()


# -----------------------
# GoogleTest Runtime Compatibility
# -----------------------
if(BUILD_TESTING_ONLY OR (CMAKE_CONFIGURATION_TYPES AND CMAKE_CFG_INTDIR STREQUAL "Testing"))
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  message(STATUS "🔄 [CMake] GoogleTest configured to use shared CRT (/MDd) to match project settings")
endif()

project(NoiseGenerator VERSION 1.0 LANGUAGES CXX)

# -----------------------
# Output folders
# -----------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# -----------------------
# Test config 
# -----------------------
option(BUILD_TESTING_ONLY "Configure CMake only for tests (no main app)" OFF)

if(BUILD_TESTING_ONLY OR (CMAKE_CONFIGURATION_TYPES AND CMAKE_CFG_INTDIR STREQUAL "Testing"))
  message(STATUS "🧪 Enabling test-only mode (BUILD_TESTING_ONLY or Testing config)")
  include(${CMAKE_SOURCE_DIR}/cmake/AddTests.cmake)
  return()
endif()

# -----------------------
# Compiler settings
# -----------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------
# Fetch external dependencies
# -----------------------
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG master
)
if(NOT glfw_POPULATED)
  message(STATUS "🔽 Downloading GLFW from GitHub...")
endif()
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
if(NOT glad_POPULATED)
  message(STATUS "🔽 Downloading GLAD from GitHub...")
endif()
FetchContent_MakeAvailable(glad)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
if(NOT imgui_POPULATED)
  message(STATUS "🔽 Downloading ImGui from GitHub...")
endif()
FetchContent_MakeAvailable(imgui)

# NativeFileDialog
FetchContent_Declare(
  nativefiledialog
  GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
  GIT_TAG master
)
if(NOT nativefiledialog_POPULATED)
  message(STATUS "🔽 Downloading NativeFileDialog from GitHub...")
endif()
FetchContent_MakeAvailable(nativefiledialog)

# INIH
FetchContent_Declare(
  inih
  GIT_REPOSITORY https://github.com/benhoyt/inih.git
  GIT_TAG r56
)
if(NOT inih_POPULATED)
  message(STATUS "🔽 Downloading INIH from GitHub...")
endif()
FetchContent_MakeAvailable(inih)

# -----------------------
# Build libraries
# -----------------------

# ImGui
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glfw_SOURCE_DIR}/include
  ${glad_SOURCE_DIR}/include
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD IMGUI_ENABLE_DOCKING)

# INIH
add_library(inih STATIC
  ${inih_SOURCE_DIR}/ini.c
  ${inih_SOURCE_DIR}/cpp/INIReader.cpp
)
target_include_directories(inih PUBLIC
  ${inih_SOURCE_DIR}
  ${inih_SOURCE_DIR}/cpp
)

# NativeFileDialog (platform-dependent)
if(WIN32)
  set(NFD_SRC
    ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
    ${nativefiledialog_SOURCE_DIR}/src/nfd_win.cpp
  )
elseif(UNIX AND NOT APPLE)
  set(NFD_SRC
    ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
    ${nativefiledialog_SOURCE_DIR}/src/nfd_gtk.c
  )
elseif(APPLE)
  set(NFD_SRC
    ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
    ${nativefiledialog_SOURCE_DIR}/src/nfd_cocoa.m
  )
endif()
add_library(nfd STATIC ${NFD_SRC})
target_include_directories(nfd PUBLIC ${nativefiledialog_SOURCE_DIR}/src/include)
target_compile_definitions(nfd PRIVATE _CRT_SECURE_NO_WARNINGS)

# -----------------------
# Find native packages
# -----------------------
find_package(OpenGL REQUIRED)

# -----------------------
# Main executable
# -----------------------
add_executable(NoiseGenerator
  src/main.cpp

  src/Application/NGApplication.cpp
  src/Application/NGApplication.h

  src/GUI/GuiManager.cpp
  src/GUI/GuiManager.h
  src/GUI/GuiUtils.h
  src/GUI/IconRegistry.h

  src/Image/ImageExporter.cpp
  src/Image/ImageExporter.h

  src/Logger/Logger.cpp
  src/Logger/Logger.h
  src/Logger/LoggerUI.h
  src/Logger/LoggerMacro.h

  src/Noise/NoiseGenerator.cpp
  src/Noise/NoiseGenerator.h
  src/Noise/NoiseMath.cpp
  src/Noise/NoiseMath.h
  src/Noise/NoisePreviewPanelUI.cpp
  src/Noise/NoisePreviewPanelUI.h
  src/Noise/NoiseTypes.h

  src/Utils/Constants.h
  src/Utils/RandomGenerator.h
  src/Utils/StringUtils.h
  src/Utils/UIUtils.h
  src/Utils/FileUtils.h

  src/Config/SettingsManager.cpp
  src/Config/SettingsManager.h

  
  src/Controller/NoisePanelController.cpp
  src/Controller/NoisePanelController.h



  src/Model/NoisePanelModel.cpp
  src/Model/NoisePanelModel.h

  src/ThirdParty/stb_image.h
  src/ThirdParty/stb_image_write.h
  src/ThirdParty/IconsFontAwesome5.h
)

# -----------------------
# Source groups (for Visual Studio)
# -----------------------
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES
  src/main.cpp

  src/Application/NGApplication.cpp
  src/Application/NGApplication.h

  src/GUI/GuiManager.cpp
  src/GUI/GuiManager.h
  src/GUI/GuiUtils.h
  src/GUI/IconRegistry.h

  src/Image/ImageExporter.cpp
  src/Image/ImageExporter.h

  src/Logger/Logger.cpp
  src/Logger/Logger.h
  src/Logger/LoggerUI.h
  src/Logger/LoggerMacro.h

  src/Noise/NoiseGenerator.cpp
  src/Noise/NoiseGenerator.h
  src/Noise/NoiseMath.cpp
  src/Noise/NoiseMath.h
  src/Noise/NoisePreviewPanelUI.cpp
  src/Noise/NoisePreviewPanelUI.h
  src/Noise/NoiseTypes.h

  src/Utils/Constants.h
  src/Utils/RandomGenerator.h
  src/Utils/StringUtils.h
  src/Utils/UIUtils.h
  src/Utils/FileUtils.h

  src/Controller/NoisePanelController.cpp
  src/Controller/NoisePanelController.h

  src/Model/NoisePanelModel.cpp
  src/Model/NoisePanelModel.h
  
  
  src/Config/SettingsManager.cpp
  src/Config/SettingsManager.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ThirdParty PREFIX "ThirdParty" FILES
  src/ThirdParty/stb_image.h
  src/ThirdParty/stb_image_write.h
  src/ThirdParty/IconsFontAwesome5.h
)

# -----------------------
# Include directories
# -----------------------
target_include_directories(NoiseGenerator PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/_deps/nativefiledialog-src/src/include
  ${inih_SOURCE_DIR}
  ${inih_SOURCE_DIR}/cpp
)
target_compile_definitions(NoiseGenerator PRIVATE _CRT_SECURE_NO_WARNINGS)

# -----------------------
# Link libraries
# -----------------------
target_link_libraries(NoiseGenerator PRIVATE
  imgui
  glfw
  glad
  OpenGL::GL
  nfd
  inih
)

# -----------------------
# Windows: enable console in Debug, hide in Release
# -----------------------
if(WIN32 AND CMAKE_CONFIGURATION_TYPES)
  set_target_properties(NoiseGenerator PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "")
  

  set_target_properties(NoiseGenerator PROPERTIES WIN32_EXECUTABLE TRUE)


  set_target_properties(NoiseGenerator PROPERTIES
    VS_CONFIGURATION_TYPE_DEBUG "Application"
    VS_CONFIGURATION_TYPE_RELEASE "Application"
  )

  set_target_properties(NoiseGenerator PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS"
  )

  message(STATUS "🪟 WIN32_EXECUTABLE: Debug = console, Release = GUI")
endif()


# -----------------------
# Visual Studio setup
# -----------------------
if(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NoiseGenerator)
endif()

# -----------------------
# Build configuration feedback
# -----------------------
message(STATUS "🚀 Configuring NoiseGenerator project")
message(STATUS "🧠 Using C++ standard: ${CMAKE_CXX_STANDARD}")

if(WIN32)
  message(STATUS "🖥  Platform: Windows")
elseif(UNIX AND NOT APPLE)
  message(STATUS "🐧 Platform: Linux")
elseif(APPLE)
  message(STATUS "🍎 Platform: macOS")
endif()

message(STATUS "📦 Dependencies successfully fetched and configured")
message(STATUS "✅ NoiseGenerator target configured and linked")

# -----------------------
# Set default build type for single-config generators
# -----------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  message(STATUS "⚙️ Default build type set to: ${CMAKE_BUILD_TYPE}")
endif()

# -----------------------
# Post-build: copy config and fonts
# -----------------------
add_custom_command(TARGET NoiseGenerator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:NoiseGenerator>/config
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_SOURCE_DIR}/config/settings.ini
          $<TARGET_FILE_DIR:NoiseGenerator>/config/settings.ini

  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:NoiseGenerator>/fonts
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/fonts
          $<TARGET_FILE_DIR:NoiseGenerator>/fonts
)
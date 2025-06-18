cmake_minimum_required(VERSION 3.11)
project(NoiseGenerator)


# --> clang-tidy / clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
  src/Noise/NoisePreviewPanel.cpp
  src/Noise/NoisePreviewPanel.h
  src/Noise/NoiseTypes.h

  src/Utils/Constants.h
  src/Utils/RandomGenerator.h
  src/Utils/StringUtils.h
  src/Utils/UIUtils.h

  src/Config/SettingsManager.cpp
  src/Config/SettingsManager.h

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
  src/Noise/NoisePreviewPanel.cpp
  src/Noise/NoisePreviewPanel.h
  src/Noise/NoiseTypes.h

  src/Utils/Constants.h
  src/Utils/RandomGenerator.h
  src/Utils/StringUtils.h
  src/Utils/UIUtils.h

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
# Visual Studio setup
# -----------------------
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NoiseGenerator)
if(WIN32)
  set_target_properties(NoiseGenerator PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# -----------------------
# Build configuration feedback
# -----------------------
option(USE_UNICODE_MESSAGES "Use emoji in build messages" ON)
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
# Post-build: copy default config
# -----------------------
add_custom_command(TARGET NoiseGenerator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:NoiseGenerator>/config
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_SOURCE_DIR}/config/settings.ini
          $<TARGET_FILE_DIR:NoiseGenerator>/config/settings.ini
)

cmake_minimum_required(VERSION 3.11)
project(NoiseGenerator)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------
# GLFW
# -----------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

# -----------------------
# GLAD (OpenGL loader)
# -----------------------
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# -----------------------
# ImGui
# -----------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        docking  
)
FetchContent_MakeAvailable(imgui)

# -----------------------
# NativeFileDialog (NFD)
# -----------------------
FetchContent_Declare(
  nativefiledialog
  GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(nativefiledialog)

# -----------------------
# INIH (INIReader)
# -----------------------
FetchContent_Declare(
  inih
  GIT_REPOSITORY https://github.com/benhoyt/inih.git
  GIT_TAG        r56
)
FetchContent_MakeAvailable(inih)

add_library(inih STATIC
  ${inih_SOURCE_DIR}/ini.c
  ${inih_SOURCE_DIR}/cpp/INIReader.cpp
)

target_include_directories(inih PUBLIC
  ${inih_SOURCE_DIR}
  ${inih_SOURCE_DIR}/cpp
)

add_library(
  nfd 
  STATIC
  ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
  ${nativefiledialog_SOURCE_DIR}/src/nfd_win.cpp
)

target_include_directories(
  nfd 
  PUBLIC
  ${nativefiledialog_SOURCE_DIR}/src/include
)

find_package(OpenGL REQUIRED)

# ImGui library
add_library(
  imgui 
  STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(
  imgui 
  PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glfw_SOURCE_DIR}/include
  ${glad_SOURCE_DIR}/include
)

target_compile_definitions(
  imgui 
  PUBLIC 
  IMGUI_IMPL_OPENGL_LOADER_GLAD IMGUI_ENABLE_DOCKING
)

# Main executable
add_executable(NoiseGenerator
  src/main.cpp
 
  src/Logger/Logger.h
  src/Logger/Logger.cpp
  src/Logger/LoggerUI.h
  src/Logger/LogCategory.h
  src/Logger/LoggerMacro.h
  
  src/Application/NGApplication.cpp
  src/Application/NGApplication.h

  src/GUI/GuiManager.cpp
  src/GUI/GuiManager.h
  src/GUI/GuiUtils.h
  src/GUI/IconRegistry.h

  src/Noise/NoiseGenerator.cpp
  src/Noise/NoiseGenerator.h
  src/Noise/NoiseMath.cpp
  src/Noise/NoiseMath.h
  src/Noise/NoiseTypes.h
  src/Noise/NoisePreviewPanel.cpp
  src/Noise/NoisePreviewPanel.h

  src/Image/ImageExporter.cpp
  src/Image/ImageExporter.h
  
  src/Utils/Constants.h
  src/Utils/RandomGenerator.h
  src/Utils/StringUtils.h
  src/Utils/UIUtils.h

  src/Config/SettingsManager.cpp
  src/Config/SettingsManager.h

  src/ThirdParty/stb_image_write.h
  src/ThirdParty/stb_image.h
  src/ThirdParty/IconsFontAwesome5.h

)

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
    src/Logger/LogCategory.h
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

target_include_directories(NoiseGenerator 
  PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/_deps/nativefiledialog-src/src/include
    ${inih_SOURCE_DIR} 
    ${inih_SOURCE_DIR}/cpp
    
)

target_include_directories(NoiseGenerator PRIVATE ${inih_SOURCE_DIR})

target_link_libraries(
  NoiseGenerator 
  PRIVATE 
  imgui 
  glfw 
  glad 
  OpenGL::GL  
  nfd
  inih
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NoiseGenerator)

if (WIN32)
  set_target_properties(NoiseGenerator PROPERTIES
    WIN32_EXECUTABLE TRUE
  )
endif()
option(USE_UNICODE_MESSAGES "Use emoji in build messages" ON)
message(STATUS "🚀 Configuring NoiseGenerator project")
message(STATUS "🧠 Using C++ standard: ${CMAKE_CXX_STANDARD}")

if (WIN32)
    message(STATUS "🖥  Platform: Windows")
elseif (UNIX AND NOT APPLE)
    message(STATUS "🐧 Platform: Linux")
elseif (APPLE)
    message(STATUS "🍎 Platform: macOS")
endif()

message(STATUS "📦 GLFW, GLAD, and ImGui successfully fetched and added")
message(STATUS "✅ NoiseGenerator target configured and linked")

add_custom_command(TARGET NoiseGenerator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:NoiseGenerator>/config
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_SOURCE_DIR}/config/settings.ini
          $<TARGET_FILE_DIR:NoiseGenerator>/config/settings.ini
)
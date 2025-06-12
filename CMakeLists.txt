cmake_minimum_required(VERSION 3.11)
project(NoiseGenerator)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

# GLAD (OpenGL loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        docking  
)
FetchContent_MakeAvailable(imgui)

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

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glfw_SOURCE_DIR}/include
  ${glad_SOURCE_DIR}/include
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD IMGUI_ENABLE_DOCKING)

# Main executable
add_executable(NoiseGenerator
  src/main.cpp
  src/Math/vector.h 
  src/Logger/Logger.h
  src/Logger/Logger.cpp
  src/Logger/LoggerUI.h
  src/Application/NGApplication.cpp
  src/Application/NGApplication.h
  src/GUI/GuiManager.cpp
  src/GUI/GuiManager.h
  src/Noise/NoiseGenerator.cpp
  src/Noise/NoiseGenerator.h
  src/Noise/NoiseMath.cpp
  src/Noise/NoiseMath.h
  src/Noise/NoiseTypes.h
  src/Noise/NoisePreviewPanel.cpp
  src/Noise/NoisePreviewPanel.h
  src/Image/ImageExporter.cpp
  src/Image/ImageExporter.h
  src/Utils/RandomUtils.h


  src/ThirdParty/stb_image_write.h
)

target_include_directories(NoiseGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(NoiseGenerator PRIVATE imgui glfw glad OpenGL::GL)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NoiseGenerator)

if (WIN32)
  set_target_properties(NoiseGenerator PROPERTIES
    WIN32_EXECUTABLE TRUE
  )
endif()
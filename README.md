# Noise Generator

An interactive C++ desktop application for generating and visualizing procedural 2D noise (including Perlin noise) with customizable parameters, preview rendering, and export options.

## ✨ Features

- ✅ Procedural 2D noise generation (Perlin + turbulence + marbling)
- ✅ Real-time GUI interface using ImGui with docking support
- ✅ Adjustable parameters: resolution, seed, roughness, marbling, frequency skips
- ✅ Turbulence distortion with exponential shifting and offsets
- ✅ Live preview of generated noise textures
- ✅ Export to `.png` and `.tga` formats
- ✅ Built-in logger (with UI panel and save-to-file functionality)
- ✅ Asynchronous generation with visual progress

---

## 🧩 Dependencies

| Library          | Purpose                  |
|------------------|---------------------------|
| [GLFW](https://www.glfw.org/)         | Window & input handling      |
| [GLAD](https://glad.dav1d.de/)        | OpenGL function loader       |
| [Dear ImGui](https://github.com/ocornut/imgui) | GUI rendering                |
| stb_image_write  | PNG & TGA image saving   |
| OpenGL 3.3+       | GPU rendering backend     |

You can integrate these manually or using CMake/your build system of choice.

# Noise Generator
![C++17](https://img.shields.io/badge/language-C%2B%2B17-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-4.3-green)
![GLFW](https://img.shields.io/badge/Windowing-GLFW-0099cc)
![ImGui](https://img.shields.io/badge/GUI-Dear%20ImGui-blueviolet)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
![License: MIT](https://img.shields.io/badge/license-MIT-blue)

An interactive C++ desktop application for generating and visualizing procedural 2D noise (including Perlin noise) with customizable parameters, preview rendering, and export options.

## ✨ Features

- ✅ Procedural 2D noise generation (Perlin + turbulence + marbling)
- ✅ Real-time GUI interface using ImGui with docking support
- ✅ Adjustable parameters: resolution, seed, roughness, marbling, frequency skips
- ✅ Turbulence distortion with exponential shifting and offsets
- ✅ Live preview of generated noise textures
- ✅ Export to `.png`, `.tga`, `.bmp` and `.jpg` formats
- ✅ Built-in logger (with UI panel and save-to-file functionality)
- ✅ Asynchronous generation with visual progress

---

## 📸 Preview

![Noise Generator Preview](docs/noise_generator2.gif)

---
## 🧩 Dependencies

| Library          | Purpose                  |
|------------------|---------------------------|
| [GLFW](https://www.glfw.org/)         | Window & input handling      |
| [GLAD](https://glad.dav1d.de/)        | OpenGL function loader       |
| [Dear ImGui](https://github.com/ocornut/imgui) | GUI rendering                |
| [Native File Dialog](https://github.com/mlabbe/nativefiledialog) | Native File Dialog     |
| [inih](https://github.com/benhoyt/inih)        |  .INI file parser     |
| stb_image_write  | PNG & TGA image saving   |
| OpenGL 4.3+       | GPU rendering backend     |
| IconsFontAwesome5 | icons for ImGui         |
---

## 🛠 Building with CMake
This project uses CMake ≥ 3.11 and automatically fetches its dependencies (GLFW, GLAD, ImGui) via FetchContent. No manual installation required.

🔧 Build Instructions

Windows (Visual Studio):
```
git clone https://github.com/VaeDeveloper/NoiseGenerator.git
cd NoiseGenerator
cmake -B build -G "Visual Studio 17 2022" -A x64
```

✅ Requirements
CMake ≥ 3.11
C++17 compatible compiler
OpenGL 4.3+ support
Internet connection (for dependency fetching)


## 🧠 Roadmap / Ideas

- [ ] Save/load generation presets (e.g., JSON)
- [ ] GPU acceleration (OpenGL / compute shaders) 
- [ ] Export to .bmp / .jpg - DONE !!! 
- [ ] Multi-threaded tile-based noise generation
- [ ] Native File Dialog for save image - DONE !!!
- [ ] Saving Layout's

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

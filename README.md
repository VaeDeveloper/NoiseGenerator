# Noise Generator
![C++17](https://img.shields.io/badge/language-C%2B%2B17-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-4.3-green)
![GLFW](https://img.shields.io/badge/Windowing-GLFW-0099cc)
![ImGui](https://img.shields.io/badge/GUI-Dear%20ImGui-blueviolet)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
![License: MIT](https://img.shields.io/badge/license-MIT-blue)

An interactive C++ desktop application for generating and visualizing procedural **2D noise** with customizable parameters, real-time preview rendering, and flexible preset management.

## ✨ Features

- ✅ Procedural 2D noise generation (Perlin + turbulence + marbling)
- ✅ Real-time GUI interface using ImGui with docking support
- ✅ Adjustable parameters: resolution, seed, roughness, marbling, frequency skips
- ✅ Turbulence distortion with exponential shifting and offsets
- ✅ Support for **preset saving, loading, and deletion** via `.json`
- ✅ Live preview of generated noise textures
- ✅ Export to `.png`, `.tga`, `.bmp` and `.jpg` formats
- ✅ Built-in logger (with UI panel and save-to-file functionality)
- ✅ Asynchronous generation with visual progress

---

## 📸 Preview

![Noise Generator Preview](docs/noise_generator2.gif)

---

## 🧩 Supported Noise Types

Available in the `NoiseType`:

- Value
- Perlin
- Simplex
- FBM (Fractal Brownian Motion)
- Worley
- Ridged
- Billow
- DomainWarp
- Cellular
- Voronoi
- Gabor
- White / Blue / Red / Pink noise
- OpenSimplex
- SuperSimplex
- IQNoise
- SwissTurbulence
- JordanNoise

> 📦 Easily extendable via `NoiseType`, generator dispatch, and serialization logic.

---

## 💾 Presets System

Each preset saves the full set of `NoiseProperties`, resolution, and noise type:

- ✅ Save current parameters to a `.json` preset
- ✅ Load and apply presets instantly via combo box
- ✅ Delete presets via GUI
- ✅ Stored in `presets/` folder as JSON files

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

## 🛠 Building & Development

This project uses **CMake ≥ 3.11** and automatically fetches all dependencies via `FetchContent`. No manual setup is required.

### ✅ Requirements

- CMake ≥ 3.11
- C++17 compatible compiler
- OpenGL 4.3+ capable GPU
- Internet connection (to fetch dependencies)

### 🔧 Quick Start (Windows / Visual Studio)

To build and run the project with minimal effort:

```bash
git clone https://github.com/VaeDeveloper/NoiseGenerator.git
cd NoiseGenerator
dev_ops\generate_project.bat
dev_ops\build_project.bat
```

## 🧪 Running Tests

You can configure and build tests using a special flag:

```bash
dev_ops\generate_project.bat
change 3 - testing options
```
---
## 🧠 Roadmap / Ideas

- [x] Export to `.bmp` / `.jpg`
- [x] Native File Dialog (save/load image)
- [x] Save/load presets as `.json`
- [ ] Multi-threaded generation (tile-based)
- [ ] GPU acceleration (OpenGL compute shaders)
- [ ] Save/restore UI layout (ImGui settings)
- [ ] CLI interface for headless noise export

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

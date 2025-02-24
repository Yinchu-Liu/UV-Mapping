# UV Mapping Project

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3-red.svg)](https://www.opengl.org/)
[![GLFW](https://img.shields.io/badge/GLFW-3.3.8-green.svg)](https://www.glfw.org/)
[![GLM](https://img.shields.io/badge/GLM-0.9.9.8-blue.svg)](https://github.com/g-truc/glm)

This project demonstrates UV mapping in OpenGL using C++. It provides a simple framework for loading 3D models, applying textures, and rendering them with basic lighting.

## Features

- OpenGL 3.3 Core Profile
- GLFW for window management
- GLM for mathematics
- Model loading with UV coordinates
- Texture mapping
- Basic Phong lighting

## Project Structure

```
UVMapping/
├── assets/              # Asset files (models, textures)
├── build/              # Build output directory
├── include/            # Header files
│   ├── Mesh.h         # Mesh handling
│   ├── Renderer.h     # Rendering system
│   ├── Shader.h       # Shader management
│   └── Texture.h      # Texture handling
├── shaders/           # GLSL shader files
│   ├── vertex_shader.glsl
│   └── fragment_shader.glsl
├── src/               # Source files
│   ├── Mesh.cpp
│   ├── Renderer.cpp
│   ├── Shader.cpp
│   └── Texture.cpp
├── main.cpp           # Application entry point
├── CMakeLists.txt    # CMake build configuration
├── LICENSE           # MIT License
└── README.md         # This file
```

## Dependencies

- OpenGL 3.3+
- GLFW 3.3.8
- GLM 0.9.9.8
- stb_image (included)

## Building

This project uses CMake as its build system. To build the project:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

1. Place your 3D models in the `assets/models/` directory
2. Place your textures in the `assets/textures/` directory
3. Run the executable
4. The application will load the model and texture, applying UV mapping

## License

This project is open source and available under the MIT License.

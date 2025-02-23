# UV Mapping Project

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
UVMappingProject/
│── src/
│   ├── main.cpp           # Application entry point
│   ├── Renderer.cpp       # OpenGL rendering setup and main loop
│   ├── Renderer.h
│   ├── Mesh.cpp          # 3D model loading and rendering
│   ├── Mesh.h
│   ├── Texture.cpp       # Texture loading and management
│   ├── Texture.h
│   ├── Shader.cpp        # GLSL shader program management
│   ├── Shader.h
│── shaders/
│   ├── vertex_shader.glsl   # Vertex shader
│   ├── fragment_shader.glsl # Fragment shader
│── assets/
│   ├── models/           # 3D model files
│   ├── textures/         # Texture files
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

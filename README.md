# 💧 SPH Fluid Simulation – IGR200

This project is a basic Smoothed Particle Hydrodynamics (SPH) fluid simulator. It aims to simulate particle-based fluid behavior in real time using OpenGL. The current version runs on the CPU, with planned support for GPU compute shaders.

---

## 📦 Installation

All required libraries are included. You’ll only need `cmake` and a C++ compiler.

### Linux

```bash
cd <project_folder>
cmake .
make
```

⚠️ Note: Building in a separate directory (e.g., `build/`) may break shader or texture loading unless paths are adjusted accordingly.

### Windows

* **With Visual Studio / VSCode**: Open the folder, let CMake configure the project, then build it.
* **With MinGW**:

```bash
cmake -G "MinGW Makefiles" .
make        # or mingw32-make
```

### macOS

Not tested, but should work with CMake and OpenGL installed. Some path adjustments may be needed.

---

## 🌊 Overview

This simulator visualizes a set of particles interacting under SPH-based fluid dynamics. It includes basic physics: gravity, density/pressure calculations, collision handling with scene boundaries, and real-time rendering using OpenGL.

### Current Features

* CPU-based SPH particle simulation
* Uniform grid-based neighbor search
* Gravity and boundary collision handling
* Rendering with Phong-shaded spheres
* Configurable camera movement and simulation controls
* Support for visual and invisible plane constraints

---

## 🎮 Controls

* `P`: Pause/unpause simulation
* `F`: Toggle mouse-controlled camera
* `W/S`: Move forward/backward
* `A/D`: Move left/right
* `Space` / `Ctrl`: Move up/down
* `Shift` / `Alt`: Adjust movement speed
* `ESC`: Exit the simulation

---

## 🛠 Project Structure
// I will write this later

---

## 📝 Roadmap

* [ ] Integrate GPU-based compute shader acceleration
* [ ] Improve physical realism using standard SPH kernels
* [ ] Optimize simulation performance
* [ ]  Add GUI for real time debugging

# OpenGL Living Room Scene

A C++ 3D scene assembled from reusable primitive meshes and rendered with OpenGL. The scene includes a floor, lamp, television and stand, and bookshelf, with textured materials, Phong lighting, and an interactive camera.

## What I implemented

- Decomposed reference objects into boxes, cylinders, tapered cylinders, and planes.
- Applied scale, rotation, and translation in a consistent transform order.
- Loaded and mapped four textures across scene objects.
- Defined material properties and two light sources for Phong shading.
- Implemented keyboard, mouse-look, scroll-speed, and perspective/orthographic camera controls.
- Organized scene preparation separately from per-frame rendering.

## Source map

| File | Responsibility |
|---|---|
| `src/SceneManager.cpp` | Textures, materials, lights, transforms, and object rendering |
| `src/SceneManager.h` | Scene data and rendering interface |
| `src/ViewManager.cpp` | Camera state and input handling |
| `src/ViewManager.h` | View manager interface |
| `src/MainCode.cpp` | Window setup, shaders, render loop, and cleanup |

## Build context

These files run inside the SNHU OpenGL starter framework, which supplies the shared shader, primitive-mesh, camera, GLEW, GLFW, GLM, and texture-loading utilities. The repository exposes the complete source I authored and modified; the course-provided framework is not redistributed here.

## Design approach

I built the scene incrementally: geometry first, then transforms, textures, lighting, and camera behavior. Running after each meaningful change made visual regressions easy to isolate. The project made matrix order and the rendering pipeline concrete in a way that higher-level engine tooling normally hides.

The original design reflection is preserved in `docs/design-decisions.docx`.

---

Built for CS-330 Computational Graphics and Visualization at Southern New Hampshire University.

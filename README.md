## 🌌 Solar System Visualization (Interactive Graphics Project)

This repository contains the source code for an interactive 3D graphics application that implements a simplified artificial solar system using modern **OpenGL**. The project was developed as a requirement for a computer graphics project at **Télécom Paris**.

---

### Project Overview & Core Implementation

The primary goal of this project was to implement fundamental concepts of interactive 3D rendering, including mesh definition, lighting, texturing, and animation.

The initial implementation involved the following steps:

1.  **Mesh Definition:** Creation and definition of a single `Sphere Mesh` object.
2.  **Rendering:** Implementation of custom **Shading** and application of **Textures** to the sphere mesh.
3.  **Expansion:** Addition of multiple spheres to represent planets and satellites.
4.  **Animation:** Implementation of an animation loop to simulate orbital and rotational movements, showcasing the basic interactions among the celestial objects.

---

### Extensions

The project was extended beyond the basic requirements to achieve a more comprehensive and realistic visualization:

* **Expanded Planet Set:** Two additional planets, **Mars** and **Venus**, were integrated into the simulation, using proportions adequate for a visually balanced system.
* **Realistic Environment:** The default background color was changed to a **darker tone** to more accurately simulate the appearance of deep space.
* **Optimized Camera Controls:** The `KeyCallback()` function was updated to provide intuitive, keyboard-driven camera navigation.

---

### Controls

The application uses the keyboard for real-time camera control:

| Key | Function | Description |
| :--- | :--- | :--- |
| **↑** (Up Arrow) | Zoom In | Moves the camera closer to the solar system center. |
| **↓** (Down Arrow) | Zoom Out | Moves the camera further away from the solar system center. |
| **→** (Right Arrow) | Rotate Right | Rotates the entire scene around the vertical axis. |
| **←** (Left Arrow) | Rotate Left | Rotates the entire scene around the vertical axis. |

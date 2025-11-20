# Final Project: 3D Campus Event Hall Visualizer

### Name: Neha Sanjay Kolambe (CSCI 5229)

---

## Project Status Summary

This project now features a fully interactive 3D event-hall environment built in OpenGL, including movable furniture, mouse-based object picking, basic AABB collision detection, dynamic lighting, and smooth first-person camera navigation. The codebase has been modularized into dedicated files for geometry, object rendering, lighting, controls, mouse picking, and collision logic, making the system easier to read and extend. The scene also includes complex-shaped furniture that goes beyond basic cubes and cylinders.

## Future Work

Upcoming improvements include adding more furniture and decorative props, expanding lighting options, introducing an accurate top-down planning mode, and refining collision systems. The current AABB collision model is functional but limited, allowing slight overlaps; future work will focus on more robust object–object constraints and FPV collision detection so the user cannot clip through chairs, tables, or stage boundaries.

---

## How to Run

1. Unzip the archive:

   ```bash
   unzip -v final.zip
   ```
2. Clean previous build files:

   ```bash
   make clean
   ```
3. Compile the program:

   ```bash
   make
   ```
4. Run the executable:

   ```bash
   ./final
   ```

---

## Controls

### General

- **Esc** – Quit program
- **m / M** – Toggle view mode (Projection / First-person)
- **0** – Reset entire scene (camera, light, FPV, selection)

### Projection Mode (mode = 0)

#### Camera Movement

- **(** – Move camera backward
- **)** – Move camera forward
- **+ / =** – Zoom in (decrease FOV)
- **- / _** – Zoom out (increase FOV)

#### Camera Rotation

- **Arrow Left** – Rotate view left
- **Arrow Right** – Rotate view right
- **Arrow Up** – Rotate view upward (clamped to +90°)
- **Arrow Down** – Rotate view downward (clamped to –90°)

### First-Person Mode (mode = 1)

#### Movement

- **W / w** – Move forward
- **S / s** – Move backward
- **A / a** – Strafe left
- **D / d** – Strafe right

#### Look Around

- **Arrow Left** – Look left
- **Arrow Right** – Look right
- **Arrow Up** – Look up (max +60°)
- **Arrow Down** – Look down (min –60°)

### Lighting Controls

#### Light Motion & Modes

- **l / L** – Toggle automatic light motion
- **b / B** – Cycle light modes (0 = Off, 1 = Normal, 2 = Lamp-shade mode)

#### Light Orbit Rotation

- **[** – Rotate light left
- **]** – Rotate light right

#### Light Radius

- **{** – Decrease light radius
- **}** – Increase light radius (max = 40)

#### Light Height

- **y** – Lower light
- **Y** – Raise light (max = 30)

### Object Interaction

#### Object Rotation

- **r** – Rotate selected object clockwise (+15°)
- **R** – Rotate selected object counter-clockwise (–15°)

#### Mouse-Based Actions

- **Left Mouse Button Down** – Select object
- **Drag** – Move selected object on ground plane
- **Release** – Place object

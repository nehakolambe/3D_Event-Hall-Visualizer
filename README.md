# Final Project: 3D Campus Event Hall Visualizer

Name: Neha Sanjay Kolambe (CSCI 5229)
-------------------------------------

This project implements an interactive 3D event-hall environment using OpenGL, featuring movable furniture, basic AABB collision detection, mouse-based object picking, first-person navigation, and dynamic lighting with lamp-shade emission effects. The codebase is now modular, separating geometry, object rendering, controls, and mouse/collision logic for easier maintenance. Future work includes improving the collision system—since the current AABB approach can still allow slight overlaps or snapping—and adding an orthographic projection mode to make top-down layout planning more accurate and user-friendly.

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
- **m / M** – Toggle view mode (Projection ↔ First-person)  
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

(Object–object collision and room boundaries are enforced automatically.)
# Final Project: 3D Campus Event Hall Visualizer

### Name: Neha Sanjay Kolambe (CSCI 5229)

---

## Project Summary

This project is a fully interactive 3D event-hall environment built in OpenGL, featuring complex furniture models, dynamic lighting, object spawning, mouse-based picking through ray-object intersection tests, basic AABB/OBB collision detection, and smooth first-person navigation with collision-aware movement. The codebase is modularized across geometry, rendering, scene management, lighting, controls, mouse interaction, collision logic, and a full whiteboard drawing system that supports drawing, erasing, and background capture. Users can place, rotate, move, and remove objects, walk through the scene in FPV, toggle lighting modes, and interact with a UI-driven whiteboard for sketching or planning. This framework forms a flexible foundation for an event-layout planner or virtual staging tool, with future improvements planned for more detailed assets, enhanced lighting, a top-down planning mode, and more robust collision constraints to prevent clipping against furniture and stage structures.

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

- **Esc** - Quit program (or exit whiteboard mode if active)
- **m / M** - Toggle view mode (Prespective / First-person / Orthogonal)
- **0** - Reset entire scene (camera, light, FPV, selection)
- **/** - Save the layout as layout.csv
- **?** - Load the layout using layout.csv

### Whiteboard Mode

- **Esc** - Exit whiteboard mode
- **Left Mouse Drag** - Draw on whiteboard
- **Right Mouse Drag** - Erase
- **Double Right Click** - Clear entire whiteboard

### Prespective Mode (mode = 0)

#### Camera Movement

- **(** - decrease dim
- **)** - increase dim
- **+ / =** - decrease FOV
- **- / _** - increase FOV

#### Camera Rotation

- **Arrow Left** - Rotate view left
- **Arrow Right** - Rotate view right
- **Arrow Up** - Rotate view upward (clamped to +90°)
- **Arrow Down** - Rotate view downward (clamped to -90°)

### First-Person Mode (mode = 1)

#### Movement

- **W / w** - Move forward
- **s / S** - Move backward
- **A / a** - Move left
- **D / d** - Move right

#### Look Around

- **Arrow Left** - Look left
- **Arrow Right** - Look right
- **Arrow Up** - Look up (max +60°)
- **Arrow Down** - Look down (min -60°)

### Orthogonal Mode (mode = 2)

#### Camera Movement

- **(** - decrease dim
- **)** - increase dim

#### Camera Rotation

- **Arrow Left** - Rotate view left
- **Arrow Right** - Rotate view right
- **Arrow Up** - Rotate view upward (clamped to +90°)
- **Arrow Down** - Rotate view downward (clamped to -90°)

### Lighting Controls

#### Light Motion & Modes

- **l/L**- Toggle automatic light motion
- **b/B**- Cycle light modes (0 = Off, 1 = Normal, 2 = Lamp-shade mode)

#### Light Orbit Rotation

- **[** - Rotate light left
- **]** - Rotate light right

#### Light Radius

- **{** - Decrease light radius
- **}** - Increase light radius (max = 40)

#### Light Height

- **y** - Lower light
- **Y** - Raise light (max = 30)

### Object Interaction

#### Object Selection & Movement

- **Left Mouse Button** - Select object
- **Drag** - Move object
- **Release** - Place object
- **g / G** - Toggle snap-to-grid
- **`** - Toggle bounding-box highlight for objects

#### Object Rotation

- **r** - Rotate selected object clockwise (+15°)
- **R** - Rotate selected object counter-clockwise (-15°)

#### Deletion

- **q / Q** - Remove selected object

#### Object Spawning

- **1** - Spawn Lamp
- **2** - Spawn Event Table
- **3** - Spawn Meeting Table
- **4** - Spawn Bar Chair
- **5** - Spawn Banquet Chair
- **6** - Spawn Cocktail Table Type 1
- **7** - Spawn Cocktail Table Type 2
- **8** - Spawn Cocktail Table Type 3

---

## References

- **Shaders**
  - Ex25.c
  - https://www.rastertek.com/gl4linuxtut33.html
  - https://clockworkchilli.com/blog/8_a_fire_shader_in_glsl_for_your_webgl_games
  - https://godotshaders.com/shader/balatro-fire-shader/
- **PreHighlight selected object**
  - Ex31.c
- **Ray Intersection - Mouse**
  - https://www.cppstories.com/2012/06/select-mouse-opengl.html/
  - https://www.opengl.org/archives/resources/faq/technical/selection.htm
  - https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
  - https://stackoverflow.com/questions/28032910/opengl-picking-fastest-way
  - https://gamedev.stackexchange.com/questions/172308/c-mouse-picking-for-ray-to-plane-intersection

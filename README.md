# Final Project: 3D Campus Event Hall Visualizer

### Name: Neha Sanjay Kolambe (CSCI 5229)

---

## Project Summary

This project is a 3D interactive application for designing event layouts, featuring a fully navigable environment where users can spawn, move, rotate, and snap furniture. The technical core of the system is built on several advanced graphics and physics implementations:

* **FPV Player Physics:** Implements a first-person collision system that prevents the camera from clipping through walls or objects while intelligently handling verticality, allowing the player to seamlessly walk up stairs and onto the raised stage.
* **Rotated Object Collision:** Utilizes the Separating Axis Theorem (SAT) to accurately detect collisions between rotated furniture (Oriented Bounding Boxes), preventing objects from overlapping or clipping into each other during placement.
* **Compound Colliders:** Supports complex, non-convex object geometry by aggregating multiple sub-bounding boxes into a single compound entity, allowing for realistic collision resolution against variety of shapes.
* **Intelligent Stacking & Elevation:** Implements logic that automatically detects when furniture is dragged onto the raised stage, snapping it to the correct elevation so it sits on top of the platform rather than inside it.
* **Grid Snapping**: Implements a togglable coordinate system that aligns furniture to precise grid lines for accurate architectural layout.
* **Dynamic Scene Management:** Allows users to dynamically spawn various furniture types (chairs, tables, lamps) and delete selected objects to iteratively refine the room layout at runtime.
* **Immersive Graphics & Viewing:** Features comprehensive texturing, dynamic lighting modes (orbiting point light, standing lamp light), and three distinct camera perspectives (Perspective, FPV, Orthogonal) to visualize the space from every angle.
* **3D Ray-Casting**: Implements gluUnProject to cast rays from the 2D mouse coordinates into 3D space, allowing for precise object selection and dragging along the floor plane.
* **Procedural Modeling**: Generates complex geometry (e.g., wavy tables, curved screens) using parametric mathematical formulas (sin, exp) rather than loading external static meshes.
* **Programmable Shaders**: Features a custom GLSL fragment shader simulating animated fire using noise textures and gradient masking.
* **System Persistence**: Includes a serialization system to save and load object states (position, rotation, type) via CSV text files.

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

- **Esc** - Quit program
- **m / M** - Toggle view mode (Prespective / First-person / Orthogonal)
- **0** - Reset entire scene (camera, light, FPV, selection)
- **/** - Save the layout as layout.csv
- **?** - Load the layout using layout.csv

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

## AI Generation & Code Reuse Acknowledgement

1. **Collision Mathematics (collision.c)**

- **Used for:** The vector math implementation of the Separating Axis Theorem (functions: projectRadius, overlapOnAxis, obbOverlapXZ).
- **Why**: SAT requires projecting box corners onto multiple normal axes. I used AI to generate the specific math helpers to ensure the collision logic for rotated boxes was accurate.

2. **Procedural Geometry Math (object.c)**

- **Used for**: The "bulge and pinch" formula for the Wavy Meeting Table and the vertex generation for the Cloud and Crescent Moon shapes.
- **Why**: I used AI to generate parametric equations that would create organic, curved shapes using math functions (expf, powf) to avoid manually plotting hundreds of vertices.

---

## References

- **Shaders**
  - Ex25.c
  - https://www.rastertek.com/gl4linuxtut33.html
  - https://clockworkchilli.com/blog/8_a_fire_shader_in_glsl_for_your_webgl_games
  - https://godotshaders.com/shader/balatro-fire-shader/
- **Highlight selected object**
  - Ex31.c
- **Ray Intersection - Mouse**
  - https://www.cppstories.com/2012/06/select-mouse-opengl.html/
  - https://www.opengl.org/archives/resources/faq/technical/selection.htm
  - https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
  - https://stackoverflow.com/questions/28032910/opengl-picking-fastest-way
  - https://gamedev.stackexchange.com/questions/172308/c-mouse-picking-for-ray-to-plane-intersection
- **Collision**
  - https://dyn4j.org/2010/01/sat/
  - https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
  - https://dev.to/pratyush_mohanty_6b8f2749/the-math-behind-bounding-box-collision-detection-aabb-vs-obbseparate-axis-theorem-1gdn
- **Snap-to-grid**
  - https://www.reddit.com/r/gamemaker/comments/14i7qw/snap_to_grid_code/

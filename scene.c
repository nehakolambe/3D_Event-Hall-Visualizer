#include "CSCIx229.h"

// Stage grid and elevation settings
const float GRID_SNAP_SIZE = 5.0f;
static const float STAGE_MIN_X = -10.0f;
static const float STAGE_MAX_X = 10.0f;
static const float STAGE_MIN_Z = -30.0f;
static const float STAGE_MAX_Z = -20.0f;
static const float STAGE_HEIGHT = 2.0f;

// Door measurements
#define DOOR_WIDTH 3.0f
#define DOOR_HEIGHT 7.0f

// Curved screen display geometry
#define CURVED_SCREEN_WIDTH 35.0f
#define CURVED_SCREEN_HEIGHT 10.0f
#define CURVED_SCREEN_Y_BASE 3.5f
#define CURVED_SCREEN_RADIUS_H 25.0f
#define CURVED_SCREEN_RADIUS_V 35.0f
#define CURVED_SCREEN_Z_OFFSET 0.5f

// Toggles that can be switched at runtime
bool bboxHighlightEnabled = false;

// Object system globals
SceneObject objects[MAX_OBJECTS];
int objectCount = 0;
SceneObject *selectedObject = NULL;
SceneObject playerObj;
int dragging = 0;

// Texture IDs
unsigned int wallTex;
unsigned int floorTex;
unsigned int screenTex;
unsigned int cocktailTableTex;
unsigned int tableTex;
unsigned int stageTex;
unsigned int lampRodTex;
unsigned int lampShadeTex;
unsigned int chairCushionTex;
unsigned int chairLegTex;
unsigned int doorFrameTex;
unsigned int doorKnobTex;
unsigned int cloudTex;
unsigned int moonTex;
unsigned int starTex;
unsigned int threadTex;
unsigned int meetingTableTex;
unsigned int meetingTableLegTex;
unsigned int cocktail2Tex;
unsigned int cocktail2LegTex;
unsigned int cocktail3Tex;
unsigned int cocktail3LegTex;
unsigned int barChairCushionTex;
unsigned int barChairWoodTex;
unsigned int barChairBackTex;
unsigned int fireplaceTex;
unsigned int fireNoiseTex;

// GLSL shader for animated fire plane
int fireShader = 0;

static int positionOnStage(float x, float z);

// Returns true when a point is inside the stage area
static int positionOnStage(float x, float z)
{
    return (x >= STAGE_MIN_X && x <= STAGE_MAX_X &&
            z >= STAGE_MIN_Z && z <= STAGE_MAX_Z);
}

// Helper used when spawning the door
static void drawDoorObject(float x, float z)
{
    drawDoor(x, z, DOOR_WIDTH, DOOR_HEIGHT);
}

// Helper used when spawning the curved screen
static void drawCurvedScreenObject(float x, float z)
{
    drawCurvedScreen(x,
                     z,
                     CURVED_SCREEN_WIDTH,
                     CURVED_SCREEN_HEIGHT,
                     CURVED_SCREEN_Y_BASE,
                     CURVED_SCREEN_RADIUS_H,
                     CURVED_SCREEN_RADIUS_V,
                     CURVED_SCREEN_Z_OFFSET);
}

// Lifts objects placed on the stage platform
void scene_apply_stage_height(SceneObject *sceneObject)
{
    // Only movable objects should be adjusted
    if (!sceneObject || !sceneObject->movable)
        return;
    // If object sits on stage, raise it to stage height
    if (positionOnStage(sceneObject->x, sceneObject->z))
        sceneObject->y = STAGE_HEIGHT;
    // Otherwise keep it on floor
    else
        sceneObject->y = 0.0f;
}

// Helper used to draw quads
static void drawQuadN(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float nx, float ny, float nz,
    float r, float g, float b)
{
    GLboolean texEnabled;
    glGetBooleanv(GL_TEXTURE_2D, &texEnabled); // check if textures are active

    if (!texEnabled)
        glColor3f(r, g, b); // fallback color when no texture

    glNormal3f(nx, ny, nz);
    glBegin(GL_QUADS);

    // Emit texture coordinates only when textures are active
    if (texEnabled)
    {
        glTexCoord2f(0, 0);
        glVertex3f(x1, y1, z1);
        glTexCoord2f(1, 0);
        glVertex3f(x2, y2, z2);
        glTexCoord2f(1, 1);
        glVertex3f(x3, y3, z3);
        glTexCoord2f(0, 1);
        glVertex3f(x4, y4, z4);
    }
    else
    {
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glVertex3f(x4, y4, z4);
    }

    glEnd();
}

// Helper used to draw textured rectangle surfaces with tiling
void drawTiledSurface(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float nx, float ny, float nz,
    float tileSize)
{
    int planeOrientation;

    // Decide which axis-aligned plane this quad lies on
    if (y1 == y2)
        planeOrientation = 0; // XZ plane
    else if (z1 == z2)
        planeOrientation = 1; // XY plane
    else
        planeOrientation = 2; // ZY plane

    glNormal3f(nx, ny, nz);

    // XZ plane (floor/ceiling)
    // Handle each plane orientation separately
    if (planeOrientation == 0)
    {
        float xmin = x1, xmax = x2;
        float zmin = z1, zmax = z2;
        float y = y1;

        // Step through X and Z to draw each floor tile quad
        for (float x = xmin; x < xmax; x += tileSize)
        {
            // For each X column, iterate down the Z rows
            for (float z = zmin; z < zmax; z += tileSize)
            {
                float tileMinX = x;
                float tileMaxX = fminf(x + tileSize, xmax);
                float tileMinZ = z;
                float tileMaxZ = fminf(z + tileSize, zmax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(tileMinX, y, tileMinZ);
                glTexCoord2f(1, 0);
                glVertex3f(tileMaxX, y, tileMinZ);
                glTexCoord2f(1, 1);
                glVertex3f(tileMaxX, y, tileMaxZ);
                glTexCoord2f(0, 1);
                glVertex3f(tileMinX, y, tileMaxZ);
                glEnd();
            }
        }
    }

    // XY plane (front/back walls)
    else if (planeOrientation == 1)
    {
        float xmin = x1, xmax = x2;
        float ymin = y1, ymax = y2;
        float z = z1;

        // Sweep across X and Y to cover the wall area
        for (float x = xmin; x < xmax; x += tileSize)
        {
            // For each X column, step upward in Y
            for (float y = ymin; y < ymax; y += tileSize)
            {
                float tileMinX = x;
                float tileMaxX = fminf(x + tileSize, xmax);
                float tileMinY = y;
                float tileMaxY = fminf(y + tileSize, ymax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(tileMinX, tileMinY, z);
                glTexCoord2f(1, 0);
                glVertex3f(tileMaxX, tileMinY, z);
                glTexCoord2f(1, 1);
                glVertex3f(tileMaxX, tileMaxY, z);
                glTexCoord2f(0, 1);
                glVertex3f(tileMinX, tileMaxY, z);
                glEnd();
            }
        }
    }

    // ZY plane (left/right walls)
    else
    {
        float zmin = z1, zmax = z2;
        float ymin = y1, ymax = y2;
        float x = x1;

        // Walk across Z rows along the wall
        for (float z = zmin; z < zmax; z += tileSize)
        {
            // For each row, step up in Y to fill the column
            for (float y = ymin; y < ymax; y += tileSize)
            {
                float tileMinZ = z;
                float tileMaxZ = fminf(z + tileSize, zmax);
                float tileMinY = y;
                float tileMaxY = fminf(y + tileSize, ymax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(x, tileMinY, tileMinZ);
                glTexCoord2f(1, 0);
                glVertex3f(x, tileMinY, tileMaxZ);
                glTexCoord2f(1, 1);
                glVertex3f(x, tileMaxY, tileMaxZ);
                glTexCoord2f(0, 1);
                glVertex3f(x, tileMaxY, tileMinZ);
                glEnd();
            }
        }
    }
}

// draws a simple bounding box
void drawBBox(SceneObject *sceneObject)
{
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f); // red wireframe
    glLineWidth(2.0f);

    float rotationRadians = -sceneObject->rotation * (M_PI / 180.0f); 
    float cosRotation = cosf(rotationRadians);
    float sinRotation = sinf(rotationRadians);

    // Loop through each collision sub-box that belongs to this object
    for (int subBoxIdx = 0; subBoxIdx < sceneObject->subBoxCount; subBoxIdx++)
    {
        float xmin = sceneObject->subBox[subBoxIdx][0];
        float xmax = sceneObject->subBox[subBoxIdx][1];
        float ymin = sceneObject->subBox[subBoxIdx][2];
        float ymax = sceneObject->subBox[subBoxIdx][3];
        float zmin = sceneObject->subBox[subBoxIdx][4];
        float zmax = sceneObject->subBox[subBoxIdx][5];

        // We need the 8 corners of the box
        float corners[8][3] = {
            {xmin, ymin, zmin}, {xmax, ymin, zmin},
            {xmax, ymax, zmin}, {xmin, ymax, zmin},
            {xmin, ymin, zmax}, {xmax, ymin, zmax},
            {xmax, ymax, zmax}, {xmin, ymax, zmax}
        };

        // Transform corners manually
        // Loop over every box corner to rotate/translate it into world space
        for (int cornerIdx = 0; cornerIdx < 8; cornerIdx++) {
            float localX = corners[cornerIdx][0];
            float localY = corners[cornerIdx][1]; // Y is mostly unaffected by Y-axis rotation
            float localZ = corners[cornerIdx][2];

            // Rotate
            float rotatedX = cosRotation * localX - sinRotation * localZ;
            float rotatedZ = sinRotation * localX + cosRotation * localZ;

            // Translate
            corners[cornerIdx][0] = rotatedX + sceneObject->x;
            corners[cornerIdx][1] = localY + sceneObject->y;
            corners[cornerIdx][2] = rotatedZ + sceneObject->z;
        }

        glBegin(GL_LINES);
        // Bottom Face
        glVertex3fv(corners[0]); glVertex3fv(corners[1]);
        glVertex3fv(corners[1]); glVertex3fv(corners[5]);
        glVertex3fv(corners[5]); glVertex3fv(corners[4]);
        glVertex3fv(corners[4]); glVertex3fv(corners[0]);

        // Top Face
        glVertex3fv(corners[3]); glVertex3fv(corners[2]);
        glVertex3fv(corners[2]); glVertex3fv(corners[6]);
        glVertex3fv(corners[6]); glVertex3fv(corners[7]);
        glVertex3fv(corners[7]); glVertex3fv(corners[3]);

        // Vertical lines
        glVertex3fv(corners[0]); glVertex3fv(corners[3]);
        glVertex3fv(corners[1]); glVertex3fv(corners[2]);
        glVertex3fv(corners[5]); glVertex3fv(corners[6]);
        glVertex3fv(corners[4]); glVertex3fv(corners[7]);
        glEnd();
    }

    glPopMatrix();
}

// Adds an object to the scene array and returns a pointer to it
SceneObject *addObject(const char *name,
                       float positionX, float positionZ,
                       void (*drawFunc)(float, float),
                       int movable)
{
    // Stop if we've already reached the maximum allowed objects
    if (objectCount >= MAX_OBJECTS)
        return NULL;

    SceneObject *newObject = &objects[objectCount];

    newObject->id = objectCount;

    // Safe copy
    strncpy(newObject->name, name, sizeof(newObject->name) - 1);
    newObject->name[sizeof(newObject->name) - 1] = '\0';

    newObject->x = positionX;
    newObject->y = 0.0f;
    newObject->z = positionZ;

    newObject->scale = 1.0f;
    newObject->rotation = 0.0f;
    newObject->drawFunc = drawFunc;
    newObject->movable = movable;
    newObject->solid = 1;

    // Reset bbox
    newObject->subBoxCount = 0;
    // Clear every possible sub-box slot and each of their six values
    for (int subBoxIdx = 0; subBoxIdx < MAX_SUBBOXES; subBoxIdx++)
        // Within each slot, iterate through its six faces
        for (int faceIdx = 0; faceIdx < 6; faceIdx++)
            newObject->subBox[subBoxIdx][faceIdx] = 0.0f;

    // Track how many objects currently exist
    objectCount++;
    return newObject;
}

void scene_init(void)
{
    // Load textures
    wallTex = LoadTexBMP("textures/wall.bmp");
    floorTex = LoadTexBMP("textures/carpet.bmp");
    screenTex = LoadTexBMP("textures/screen.bmp");
    cocktailTableTex = LoadTexBMP("textures/cocktail.bmp");
    tableTex = LoadTexBMP("textures/table.bmp");
    stageTex = LoadTexBMP("textures/stage.bmp");
    lampRodTex = LoadTexBMP("textures/lamprod.bmp");
    lampShadeTex = LoadTexBMP("textures/lampshade.bmp");
    chairCushionTex = LoadTexBMP("textures/chaircushion.bmp");
    chairLegTex = LoadTexBMP("textures/chairleg.bmp");
    doorFrameTex = LoadTexBMP("textures/door.bmp");
    doorKnobTex = LoadTexBMP("textures/doorknob.bmp");
    cloudTex = LoadTexBMP("textures/cloud.bmp");
    moonTex = LoadTexBMP("textures/moon.bmp");
    starTex = LoadTexBMP("textures/star.bmp");
    threadTex = LoadTexBMP("textures/thread.bmp");
    meetingTableTex = LoadTexBMP("textures/meetingtable.bmp");
    meetingTableLegTex = LoadTexBMP("textures/meetingtableleg.bmp");
    cocktail2Tex = LoadTexBMP("textures/cocktail2.bmp");
    cocktail2LegTex = LoadTexBMP("textures/cocktail2leg.bmp");
    cocktail3Tex = LoadTexBMP("textures/cocktail3.bmp");
    cocktail3LegTex = LoadTexBMP("textures/cocktail3leg.bmp");
    barChairBackTex = LoadTexBMP("textures/barchairbackrest.bmp");
    barChairCushionTex = LoadTexBMP("textures/barchaircushion.bmp");
    barChairWoodTex = LoadTexBMP("textures/barchairwood.bmp");
    fireplaceTex = LoadTexBMP("textures/brick.bmp");
    fireNoiseTex = LoadTexBMP("textures/cloud.bmp");

    fireShader = CreateShaderProg("fire.vert", "fire.frag");

    objectCount = 0;
    scene_spawn_reset();

    // Fixed objects
    addObject("Door", 0.0f, 29.9f, drawDoorObject, 0);
    addObject("CurvedScreen", 0.0f, -30.0f, drawCurvedScreenObject, 0);
    SceneObject *fireplaceObject = addObject("Fireplace", 19.5f, -18.0f, drawFireplace, 0);
    if (fireplaceObject)
    {
        // Slightly scale the fireplace for better room presence
        fireplaceObject->scale = 1.2f;
    }

    // Event tables
    float eventTablePosX[4] = {-10, -10, 10, 10};
    float eventTablePosZ[4] = {-10, 0, -10, 0};

    // Create four default event tables at the preset positions
    for (int i = 0; i < 4; i++)
    {
        char name[32];
        sprintf(name, "EventTable%d", i + 1);
        addObject(name, eventTablePosX[i], eventTablePosZ[i], drawTable, 1);
    }

    // Event chairs
    float chairOffsetX[2] = {0.0f, 0.0f};
    float chairOffsetZ[2] = {-1.8f, 1.8f};
    float chairRotation[2] = {0.0f, 180.0f};

    // For each table, place two chairs around it
    for (int t = 0; t < 4; t++)
    {
        // Each table gets two chairs with mirrored offsets
        for (int c = 0; c < 2; c++)
        {
            char cname[32];
            sprintf(cname, "EventChair_T%d_C%d", t + 1, c + 1);

            float chairPosX = eventTablePosX[t] + chairOffsetX[c];
            float chairPosZ = eventTablePosZ[t] + chairOffsetZ[c];

            addObject(cname, chairPosX, chairPosZ, drawBanquetChair, 1);
            objects[objectCount - 1].rotation = chairRotation[c];
        }
    }

    // Cocktail tables
    float cocktailTablePosX[3] = {0, -12, 12};
    float cocktailTablePosZ[3] = {-5, -3, -3};

    addObject("Cocktail_1", cocktailTablePosX[0], cocktailTablePosZ[0], drawCocktailTable, 1);
    addObject("Cocktail_2", cocktailTablePosX[1], cocktailTablePosZ[1] + 10.0f, drawCocktailTable2, 1);
    addObject("Cocktail_3", cocktailTablePosX[2], cocktailTablePosZ[2] + 10.0f, drawCocktailTable3, 1);

    // Bar chairs
    addObject("BarChair_1",
              cocktailTablePosX[2] + 1.25f,
              cocktailTablePosZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 200.0f;

    addObject("BarChair_2",
              cocktailTablePosX[2] - 1.25f,
              cocktailTablePosZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 160.0f;

    // Lamp
    addObject("Lamp", -5.0f, 11.0f, drawLamp, 1);

    // Meeting table
    float meetingTableX = 0.0f;
    float meetingTableZ = 11.0f;

    addObject("MeetingTable", meetingTableX, meetingTableZ, drawMeetingTable, 1);

    // Meeting chairs
    float meetingChairOffsetX[2] = {-1.8f, 1.8f};
    float meetingChairFrontZ = -2.2f;
    float meetingChairBackZ = 2.2f;

    // Spawn two chairs on the front side of the meeting table
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_F%d", i + 1);

        addObject(cname,
                  meetingTableX + meetingChairOffsetX[i],
                  meetingTableZ + meetingChairFrontZ,
                  drawBanquetChair,
                  1);

        objects[objectCount - 1].rotation = 0.0f;
    }

    // Spawn two chairs on the back side of the meeting table
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_B%d", i + 1);

        addObject(cname,
                  meetingTableX + meetingChairOffsetX[i],
                  meetingTableZ + meetingChairBackZ,
                  drawBanquetChair,
                  1);

        objects[objectCount - 1].rotation = 180.0f;
    }

    // Back wall collision box
    addObject("Wall_Back", 0.0f, -30.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -20.0f;
    objects[objectCount - 1].subBox[0][1] = 20.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -1.0f;
    objects[objectCount - 1].subBox[0][5] = 1.0f;

    // Front wall collision box
    addObject("Wall_Front", 0.0f, 30.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -20.0f;
    objects[objectCount - 1].subBox[0][1] = 20.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -1.0f;
    objects[objectCount - 1].subBox[0][5] = 1.0f;

    // Left wall collision box
    addObject("Wall_Left", -20.0f, 0.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -1.0f;
    objects[objectCount - 1].subBox[0][1] = 1.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -30.0f;
    objects[objectCount - 1].subBox[0][5] = 30.0f;

    // Right wall collision box
    addObject("Wall_Right", 20.0f, 0.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -1.0f;
    objects[objectCount - 1].subBox[0][1] = 1.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -30.0f;
    objects[objectCount - 1].subBox[0][5] = 30.0f;

    // Stage collision box
    addObject("Stage", 0.0f, -25.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -10.0f;
    objects[objectCount - 1].subBox[0][1] = 10.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 2.0f;
    objects[objectCount - 1].subBox[0][4] = -5.0f;
    objects[objectCount - 1].subBox[0][5] = 5.0f;

    // Initialize bounds and apply stage height for every preloaded object
    // Visit every object so we can snap the ones that support it
    // Find the array index for the selected object
    // Render every object with its current transform
    for (int i = 0; i < objectCount; i++)
    {
        configureObjectBounds(&objects[i]);
        scene_apply_stage_height(&objects[i]);
    }

    // Optionally snap everything right after initialization
    // Only draw the placement grid overlay when snapping is enabled
    if (snapToGridEnabled)
        scene_snap_all_objects();
}

// Deletes the currently selected movable object
void scene_remove_selected_object(void)
{
    // Nothing to remove if no selection
    if (!selectedObject)
    {
        printf("No object selected to remove.\n");
        return;
    }

    int index = -1;
    for (int i = 0; i < objectCount; i++)
    {
        // Look for the array slot that matches the current selection
        if (&objects[i] == selectedObject)
        {
            index = i;
            break;
        }
    }

    // Safety: selection must exist inside the array
    if (index < 0)
        return;

    // Static fixtures cannot be deleted
    if (!objects[index].movable)
    {
        printf("Object %s cannot be removed.\n", objects[index].name);
        return;
    }

    printf("Removed %s.\n", objects[index].name);

    // Shift every later object forward to fill the gap
    for (int i = index; i < objectCount - 1; i++)
    {
        objects[i] = objects[i + 1];
        objects[i].id = i;
    }

    objectCount--;
    selectedObject = NULL;
    dragging = 0;
}

// Scene Rendering
void scene_display(void)
{
    glPushMatrix();
    lighting_update();
    glEnable(GL_LIGHTING);

    // Floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTex);
    drawTiledSurface(-20, 0, -30, 20, 0, 30, 0, 1, 0, 2.0);
    glDisable(GL_TEXTURE_2D);

    if (snapToGridEnabled)
    {
        const float minX = ROOM_MIN_X;
        const float maxX = ROOM_MAX_X;
        const float minZ = ROOM_MIN_Z;
        const float maxZ = ROOM_MAX_Z;

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.1f, 0.8f, 0.1f, 0.08f);
        glBegin(GL_QUADS);
        glVertex3f(minX, 0.01f, minZ);
        glVertex3f(maxX, 0.01f, minZ);
        glVertex3f(maxX, 0.01f, maxZ);
        glVertex3f(minX, 0.01f, maxZ);
        glEnd();

        glColor4f(0.2f, 0.9f, 0.2f, 0.35f);
        glBegin(GL_LINES);
        // Draw vertical grid lines across the floor
        for (float x = minX; x <= maxX; x += GRID_SNAP_SIZE)
        {
            glVertex3f(x, 0.02f, minZ);
            glVertex3f(x, 0.02f, maxZ);
        }
        // Draw horizontal grid lines across the floor
        for (float z = minZ; z <= maxZ; z += GRID_SNAP_SIZE)
        {
            glVertex3f(minX, 0.02f, z);
            glVertex3f(maxX, 0.02f, z);
        }
        glEnd();

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Only draw the ceiling when not inside VR mode
    // Only draw ceiling fixtures outside VR mode
    if (mode != 2)
    {
        // Ceiling
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 15, -30, 20, 15, 30, 0, -1, 0, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    // Draw the back wall only in non-VR view
    if (mode != 2)
    {
        // Back wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, -30, 20, 15, -30, 0, 0, 1, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    // Draw the front wall only in non-VR view
    if (mode != 2)
    {
        // Front wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, 30, 20, 15, 30, 0, 0, -1, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    // Draw the left wall only in non-VR view
    if (mode != 2)
    {
        // Left wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, -30, -20, 15, 30, 1, 0, 0, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    // Draw the right wall only in non-VR view
    if (mode != 2)
    {
        // Right wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(20, 0, -30, 20, 15, 30, -1, 0, 0, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    // Stage parameters
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stageTex);

    float stageTop = 2.0f;
    float stageBack = -30.0f;
    float stageFront = stageBack + 10.0f;
    float stageWidth = 10.0f;

    // Stage top
    drawQuadN(
        -stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageFront,
        0, 1, 0,
        1, 1, 1);

    // Stage front
    drawQuadN(
        -stageWidth, 0, stageFront,
        stageWidth, 0, stageFront,
        stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageFront,
        0, 0, -1,
        1, 1, 1);

    // Stage left
    drawQuadN(
        -stageWidth, 0, stageBack,
        -stageWidth, 0, stageFront,
        -stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageBack,
        -1, 0, 0,
        1, 1, 1);

    // Stage right
    drawQuadN(
        stageWidth, 0, stageFront,
        stageWidth, 0, stageBack,
        stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageFront,
        1, 0, 0,
        1, 1, 1);

    // Stage back
    drawQuadN(
        -stageWidth, 0, stageBack,
        stageWidth, 0, stageBack,
        stageWidth, stageTop, stageBack,
        -stageWidth, stageTop, stageBack,
        0, 0, 1,
        1, 1, 1);

    glDisable(GL_TEXTURE_2D);

    // Scene objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];

        glPushMatrix();
        glTranslatef(sceneObject->x, sceneObject->y, sceneObject->z);
        glRotatef(sceneObject->rotation, 0, 1, 0);
        glScalef(sceneObject->scale, sceneObject->scale, sceneObject->scale);
        glColor3f(1.0f, 1.0f, 1.0f);

        // Only draw objects that provide a callback
        if (sceneObject->drawFunc)
        {
            sceneObject->drawFunc(0, 0);
        }

        glPopMatrix();

        // Highlight bounding box of selected object when enabled
        if (bboxHighlightEnabled && selectedObject == sceneObject)
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            drawBBox(sceneObject);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
        }

        // Highlight selected object
        if (selectedObject == sceneObject)
        {
            glPushMatrix();
            glTranslatef(sceneObject->x, sceneObject->y, sceneObject->z);
            glRotatef(sceneObject->rotation, 0, 1, 0);
            glScalef(sceneObject->scale, sceneObject->scale, sceneObject->scale);

            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1, -1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glLineWidth(3.0f);
            glColor3f(1.0f, 0.0f, 0.0f);

            // Only re-draw if there is a mesh callback
            if (sceneObject->drawFunc)
                sceneObject->drawFunc(0, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);

            glPopMatrix();
        }
    }

    // Render animated fire plane if shader compiled successfully
    if (fireShader > 0)
    {
        // Render the heat plane with animated shader
        glUseProgram(fireShader);

        // shader uniforms
        float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        int timeLoc = glGetUniformLocation(fireShader, "time");
        // Upload elapsed time uniform when found
        if (timeLoc >= 0)
            glUniform1f(timeLoc, time);

        int noiseLoc = glGetUniformLocation(fireShader, "noiseTex");
        // Bind the noise texture uniform when found
        if (noiseLoc >= 0)
            glUniform1i(noiseLoc, 0);

        // bind noise texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fireNoiseTex);

        // render state changes
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);

        // position the fire plane in the fireplace
        glPushMatrix();
        glTranslatef(19.5f, 0.0f, -18.0f);
        glRotatef(-90, 0, 1, 0);
        glScalef(1.2f, 1.2f, 1.2f);
        glTranslatef(0.0f, 0.0f, 0.4f);

        drawFirePlane();
        glPopMatrix();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glUseProgram(0);
    }

    if (mode != 2)
    {
        // Ceiling lights
        glPushMatrix();
        drawCeilingShapes();
        glPopMatrix();
    }

    lighting_draw_debug_marker();
    glPopMatrix();
}

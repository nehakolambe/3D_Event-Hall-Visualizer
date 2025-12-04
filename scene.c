#include "CSCIx229.h"

// Grid snapping settings
const float GRID_SNAP_SIZE = 5.0f;

// Stage dimensions
static const float STAGE_MIN_X = -10.0f;
static const float STAGE_MAX_X = 10.0f;
static const float STAGE_MIN_Z = -30.0f;
static const float STAGE_MAX_Z = -20.0f;
static const float STAGE_HEIGHT = 2.0f;

// Door dimensions
#define DOOR_WIDTH 3.0f
#define DOOR_HEIGHT 7.0f

// Curved screen dimensions
#define CURVED_SCREEN_WIDTH 35.0f
#define CURVED_SCREEN_HEIGHT 10.0f
#define CURVED_SCREEN_Y_BASE 3.5f
#define CURVED_SCREEN_RADIUS_H 25.0f
#define CURVED_SCREEN_RADIUS_V 35.0f
#define CURVED_SCREEN_Z_OFFSET 0.5f

// Toggle to enable/disable highlight on bounding boxes
bool bboxHighlightEnabled = false;

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

// ID for the fire shader
int fireShader = 0;

// Checks if a specific (X, Z) spot is on top of the raised stage
static int positionOnStage(float x, float z)
{
    return (x >= STAGE_MIN_X && x <= STAGE_MAX_X &&
            z >= STAGE_MIN_Z && z <= STAGE_MAX_Z);
}

// Helper function to draw the door
static void drawDoorObject(float x, float z)
{
    drawDoor(x, z, DOOR_WIDTH, DOOR_HEIGHT);
}

// Helper function to draw the curved screen
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

// Automatically adjusts an object's height (Y)
// If you drag a chair onto the stage, this pops it up so it sits on the stage, not inside it
void scene_apply_stage_height(SceneObject *sceneObject)
{
    // Don't move static walls, only furniture
    if (!sceneObject || !sceneObject->movable)
        return;

    // Check if the object is inside the stage rectangle
    if (positionOnStage(sceneObject->x, sceneObject->z))
        sceneObject->y = STAGE_HEIGHT; // Lift it up
    else
        sceneObject->y = 0.0f; // Put it on the floor
}

// Helper function to draw a quad
static void drawQuadN(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float nx, float ny, float nz,
    float r, float g, float b)
{
    GLboolean texEnabled;
    glGetBooleanv(GL_TEXTURE_2D, &texEnabled);

    if (!texEnabled)
        glColor3f(r, g, b);

    glNormal3f(nx, ny, nz);
    glBegin(GL_QUADS);

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

// Helper function to draw a floor and walls
void drawTiledSurface(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float nx, float ny, float nz,
    float tileSize)
{
    int planeOrientation;

    // Check if this is a floor (XZ), a front wall (XY), or side wall (ZY)
    if (y1 == y2)
        planeOrientation = 0; // Floor/Ceiling
    else if (z1 == z2)
        planeOrientation = 1; // Front/Back Wall
    else
        planeOrientation = 2; // Left/Right Wall

    glNormal3f(nx, ny, nz);

    // Floor/Ceiling (XZ)
    if (planeOrientation == 0)
    {
        float xmin = x1, xmax = x2;
        float zmin = z1, zmax = z2;
        float y = y1;

        // Loop through the area in grid steps
        for (float x = xmin; x < xmax; x += tileSize)
        {
            // For each X column, iterate down the Z rows
            for (float z = zmin; z < zmax; z += tileSize)
            {
                // Calculate corners
                float tileMinX = x;
                float tileMaxX = fminf(x + tileSize, xmax);
                float tileMinZ = z;
                float tileMaxZ = fminf(z + tileSize, zmax);

                // Draw the tile
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

    // Front/Back Walls (XY)
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
                // Calculate corners
                float tileMinX = x;
                float tileMaxX = fminf(x + tileSize, xmax);
                float tileMinY = y;
                float tileMaxY = fminf(y + tileSize, ymax);

                // Draw the tile
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

    // Side Walls (ZY)
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
                // Calculate corners
                float tileMinZ = z;
                float tileMaxZ = fminf(z + tileSize, zmax);
                float tileMinY = y;
                float tileMaxY = fminf(y + tileSize, ymax);

                // Draw the tile
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

// Draw the highlighted bounding box for an object
void drawBBox(SceneObject *sceneObject)
{
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);

    // Rotation angle
    float rotationDegrees = -sceneObject->rotation;
    float cosRotation = Cos(rotationDegrees);
    float sinRotation = Sin(rotationDegrees);

    // Loop through every subbox
    for (int subBoxIdx = 0; subBoxIdx < sceneObject->subBoxCount; subBoxIdx++)
    {
        // Get raw box dimensions
        float xmin = sceneObject->subBox[subBoxIdx][0];
        float xmax = sceneObject->subBox[subBoxIdx][1];
        float ymin = sceneObject->subBox[subBoxIdx][2];
        float ymax = sceneObject->subBox[subBoxIdx][3];
        float zmin = sceneObject->subBox[subBoxIdx][4];
        float zmax = sceneObject->subBox[subBoxIdx][5];

        // Define the 8 corners of the box in local space
        float corners[8][3] = {
            {xmin, ymin, zmin}, {xmax, ymin, zmin}, {xmax, ymax, zmin}, {xmin, ymax, zmin}, {xmin, ymin, zmax}, {xmax, ymin, zmax}, {xmax, ymax, zmax}, {xmin, ymax, zmax}};

        // Rotate and move each corner to where the object actually is in the room
        for (int cornerIdx = 0; cornerIdx < 8; cornerIdx++)
        {
            float localX = corners[cornerIdx][0];
            float localY = corners[cornerIdx][1];
            float localZ = corners[cornerIdx][2];

            // Apply rotation formula
            float rotatedX = cosRotation * localX - sinRotation * localZ;
            float rotatedZ = sinRotation * localX + cosRotation * localZ;

            // Apply translation (position)
            corners[cornerIdx][0] = rotatedX + sceneObject->x;
            corners[cornerIdx][1] = localY + sceneObject->y;
            corners[cornerIdx][2] = rotatedZ + sceneObject->z;
        }

        // Draw lines connecting the corners
        glBegin(GL_LINES);
        // Bottom Square
        glVertex3fv(corners[0]);
        glVertex3fv(corners[1]);
        glVertex3fv(corners[1]);
        glVertex3fv(corners[5]);
        glVertex3fv(corners[5]);
        glVertex3fv(corners[4]);
        glVertex3fv(corners[4]);
        glVertex3fv(corners[0]);

        // Top Square
        glVertex3fv(corners[3]);
        glVertex3fv(corners[2]);
        glVertex3fv(corners[2]);
        glVertex3fv(corners[6]);
        glVertex3fv(corners[6]);
        glVertex3fv(corners[7]);
        glVertex3fv(corners[7]);
        glVertex3fv(corners[3]);

        // Vertical Pillars
        glVertex3fv(corners[0]);
        glVertex3fv(corners[3]);
        glVertex3fv(corners[1]);
        glVertex3fv(corners[2]);
        glVertex3fv(corners[5]);
        glVertex3fv(corners[6]);
        glVertex3fv(corners[4]);
        glVertex3fv(corners[7]);
        glEnd();
    }

    glPopMatrix();
}

// Creates a new object and puts it in the list
SceneObject *addObject(const char *name,
                       float positionX, float positionZ,
                       void (*drawFunc)(float, float),
                       int movable)
{
    // Check if we can add more objects
    if (objectCount >= MAX_OBJECTS)
        return NULL;

    SceneObject *newObject = &objects[objectCount];

    newObject->id = objectCount;

    // Set Name
    strncpy(newObject->name, name, sizeof(newObject->name) - 1);
    newObject->name[sizeof(newObject->name) - 1] = '\0';

    // Set Position
    newObject->x = positionX;
    newObject->y = 0.0f;
    newObject->z = positionZ;

    // Set Defaults
    newObject->scale = 1.0f;
    newObject->rotation = 0.0f;
    newObject->drawFunc = drawFunc;
    newObject->movable = movable;
    newObject->solid = 1;

    // Clear any collision data
    newObject->subBoxCount = 0;
    for (int subBoxIdx = 0; subBoxIdx < MAX_SUBBOXES; subBoxIdx++)
        for (int faceIdx = 0; faceIdx < 6; faceIdx++)
            newObject->subBox[subBoxIdx][faceIdx] = 0.0f;

    // Increment count so next object goes in the next slot
    objectCount++;
    return newObject;
}

void scene_init(void)
{
    // Load Textures
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

    // Load the fire animation shader
    fireShader = CreateShaderProg("fire.vert", "fire.frag");

    objectCount = 0;
    scene_spawn_reset();

    // Spawn fixed objects
    addObject("Door", 0.0f, 29.9f, drawDoorObject, 0);
    addObject("CurvedScreen", 0.0f, -30.0f, drawCurvedScreenObject, 0);
    SceneObject *fireplaceObject = addObject("Fireplace", 19.5f, -18.0f, drawFireplace, 0);
    if (fireplaceObject)
    {
        fireplaceObject->scale = 1.2f;
    }

    // Place 4 event tables
    float eventTablePosX[4] = {-10, -10, 10, 10};
    float eventTablePosZ[4] = {-10, 0, -10, 0};

    for (int i = 0; i < 4; i++)
    {
        char name[32];
        sprintf(name, "EventTable%d", i + 1);
        addObject(name, eventTablePosX[i], eventTablePosZ[i], drawTable, 1);
    }

    // Place 2 chairs at each event table
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

    // Place Cocktail Tables
    float cocktailTablePosX[3] = {0, -12, 12};
    float cocktailTablePosZ[3] = {-5, -3, -3};

    addObject("Cocktail_1", cocktailTablePosX[0], cocktailTablePosZ[0], drawCocktailTable, 1);
    addObject("Cocktail_2", cocktailTablePosX[1], cocktailTablePosZ[1] + 10.0f, drawCocktailTable2, 1);
    addObject("Cocktail_3", cocktailTablePosX[2], cocktailTablePosZ[2] + 10.0f, drawCocktailTable3, 1);

    // Place Bar Chairs near Cocktail Table 3
    addObject("BarChair_1",
              cocktailTablePosX[2] + 1.25f,
              cocktailTablePosZ[2] + 11.8f,
              drawBarChairObj, 1);
    objects[objectCount - 1].rotation = 200.0f;

    addObject("BarChair_2",
              cocktailTablePosX[2] - 1.25f,
              cocktailTablePosZ[2] + 11.8f,
              drawBarChairObj, 1);
    objects[objectCount - 1].rotation = 160.0f;

    // Place Lamp
    addObject("Lamp", -5.0f, 11.0f, drawLamp, 1);

    // Place Meeting Table
    float meetingTableX = 0.0f;
    float meetingTableZ = 11.0f;
    addObject("MeetingTable", meetingTableX, meetingTableZ, drawMeetingTable, 1);

    // Place Meeting Chairs (Front and Back)
    float meetingChairOffsetX[2] = {-1.8f, 1.8f};
    float meetingChairFrontZ = -2.2f;
    float meetingChairBackZ = 2.2f;

    // Place 2 chairs on the front side of the meeting table
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_F%d", i + 1);
        addObject(cname, meetingTableX + meetingChairOffsetX[i], meetingTableZ + meetingChairFrontZ, drawBanquetChair, 1);
        objects[objectCount - 1].rotation = 0.0f;
    }

    // Place 2 chairs on the back side of the meeting table
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_B%d", i + 1);
        addObject(cname, meetingTableX + meetingChairOffsetX[i], meetingTableZ + meetingChairBackZ, drawBanquetChair, 1);
        objects[objectCount - 1].rotation = 180.0f;
    }

    // Collision box for walls
    // Back Wall
    addObject("Wall_Back", 0.0f, -30.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -20.0f;
    objects[objectCount - 1].subBox[0][1] = 20.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -1.0f;
    objects[objectCount - 1].subBox[0][5] = 1.0f;

    // Front Wall
    addObject("Wall_Front", 0.0f, 30.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -20.0f;
    objects[objectCount - 1].subBox[0][1] = 20.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -1.0f;
    objects[objectCount - 1].subBox[0][5] = 1.0f;

    // Left Wall
    addObject("Wall_Left", -20.0f, 0.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -1.0f;
    objects[objectCount - 1].subBox[0][1] = 1.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -30.0f;
    objects[objectCount - 1].subBox[0][5] = 30.0f;

    // Right Wall
    addObject("Wall_Right", 20.0f, 0.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -1.0f;
    objects[objectCount - 1].subBox[0][1] = 1.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 15.0f;
    objects[objectCount - 1].subBox[0][4] = -30.0f;
    objects[objectCount - 1].subBox[0][5] = 30.0f;

    // Stage
    addObject("Stage", 0.0f, -25.0f, NULL, 0);
    objects[objectCount - 1].subBoxCount = 1;
    objects[objectCount - 1].subBox[0][0] = -10.0f;
    objects[objectCount - 1].subBox[0][1] = 10.0f;
    objects[objectCount - 1].subBox[0][2] = 0.0f;
    objects[objectCount - 1].subBox[0][3] = 2.0f;
    objects[objectCount - 1].subBox[0][4] = -5.0f;
    objects[objectCount - 1].subBox[0][5] = 5.0f;

    // Loop through everything we just created and configure bounding boxes and stage heights
    for (int i = 0; i < objectCount; i++)
    {
        configureObjectBounds(&objects[i]);
        scene_apply_stage_height(&objects[i]);
    }

    // If Grid Snap is enabled in code, run it to align everything perfectly
    if (snapToGridEnabled)
        scene_snap_all_objects();
}

// Deletes currently selected object
void scene_remove_selected_object(void)
{
    // Can't delete if the obejct is not selected
    if (!selectedObject)
    {
        printf("No object selected to remove.\n");
        return;
    }

    // Find where this object is in the array
    int index = -1;
    for (int i = 0; i < objectCount; i++)
    {
        // Look for the array that matches the current selection
        if (&objects[i] == selectedObject)
        {
            index = i;
            break;
        }
    }

    // Selection must exist inside the array
    if (index < 0)
        return;

    // Don't allow user to delete walls or the door
    if (!objects[index].movable)
    {
        printf("Object %s cannot be removed.\n", objects[index].name);
        return;
    }

    printf("Removed %s.\n", objects[index].name);

    // Delete it by shifting all subsequent objects left by one slot
    for (int i = index; i < objectCount - 1; i++)
    {
        objects[i] = objects[i + 1];
        objects[i].id = i;
    }

    objectCount--;
    selectedObject = NULL; // Clear selection
    dragging = 0;
}

// The Main Drawing Loop: Renders the scene
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

    // Draw grid (if enabled)
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

        // Draw grid lines
        glColor4f(0.2f, 0.9f, 0.2f, 0.35f);
        glBegin(GL_LINES);
        // Vertical lines
        for (float x = minX; x <= maxX; x += GRID_SNAP_SIZE)
        {
            glVertex3f(x, 0.02f, minZ);
            glVertex3f(x, 0.02f, maxZ);
        }
        // Horizontal lines
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

    int orthoView = (mode == 2);

    // Draw ceiling (translucent in orthogonal mode)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    if (orthoView)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
        glDepthMask(GL_FALSE);
    }
    drawTiledSurface(-20, 15, -30, 20, 15, 30, 0, -1, 0, 2.0);
    if (orthoView)
    {
        glDepthMask(GL_TRUE);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glDisable(GL_TEXTURE_2D);

    // Draw walls (translucent in orthogonal mode)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    if (orthoView)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
        glDepthMask(GL_FALSE);
    }

    // Back
    drawTiledSurface(-20, 0, -30, 20, 15, -30, 0, 0, 1, 2.0);
    // Front
    drawTiledSurface(-20, 0, 30, 20, 15, 30, 0, 0, -1, 2.0);
    // Left
    drawTiledSurface(-20, 0, -30, -20, 15, 30, 1, 0, 0, 2.0);
    // Right
    drawTiledSurface(20, 0, -30, 20, 15, 30, -1, 0, 0, 2.0);

    if (orthoView)
    {
        glDepthMask(GL_TRUE);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    glDisable(GL_TEXTURE_2D);

    // Draw stage
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stageTex);

    float stageTop = 2.0f;
    float stageBack = -30.0f;
    float stageFront = stageBack + 10.0f;
    float stageWidth = 10.0f;

    // Stage Top
    drawQuadN(
        -stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageFront,
        0, 1, 0, 1, 1, 1);

    // Stage Front Face
    drawQuadN(
        -stageWidth, 0, stageFront,
        stageWidth, 0, stageFront,
        stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageFront,
        0, 0, -1, 1, 1, 1);

    // Stage Left Face
    drawQuadN(
        -stageWidth, 0, stageBack,
        -stageWidth, 0, stageFront,
        -stageWidth, stageTop, stageFront,
        -stageWidth, stageTop, stageBack,
        -1, 0, 0, 1, 1, 1);

    // Stage Right Face
    drawQuadN(
        stageWidth, 0, stageFront,
        stageWidth, 0, stageBack,
        stageWidth, stageTop, stageBack,
        stageWidth, stageTop, stageFront,
        1, 0, 0, 1, 1, 1);

    // Stage Back Face
    drawQuadN(
        -stageWidth, 0, stageBack,
        stageWidth, 0, stageBack,
        stageWidth, stageTop, stageBack,
        -stageWidth, stageTop, stageBack,
        0, 0, 1, 1, 1, 1);

    glDisable(GL_TEXTURE_2D);

    // Draw objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];

        glPushMatrix();
        // Move to object location
        glTranslatef(sceneObject->x, sceneObject->y, sceneObject->z);
        glRotatef(sceneObject->rotation, 0, 1, 0);
        glScalef(sceneObject->scale, sceneObject->scale, sceneObject->scale);
        glColor3f(1.0f, 1.0f, 1.0f);

        // Call the draw function
        if (sceneObject->drawFunc)
        {
            sceneObject->drawFunc(0, 0);
        }

        glPopMatrix();

        // Highlight boundingbox if enabled
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

            if (sceneObject->drawFunc)
                sceneObject->drawFunc(0, 0);

            // Restore normal drawing mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);

            glPopMatrix();
        }
    }

    // Draw fire
    if (fireShader > 0)
    {
        glUseProgram(fireShader);

        // Get time
        float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        int timeLoc = glGetUniformLocation(fireShader, "time");
        if (timeLoc >= 0)
            glUniform1f(timeLoc, time);

        // Bind noise texture
        int noiseLoc = glGetUniformLocation(fireShader, "noiseTex");
        if (noiseLoc >= 0)
            glUniform1i(noiseLoc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fireNoiseTex);

        // Make fire semi-transparent
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);

        // Position fire inside the fireplace
        glPushMatrix();
        glTranslatef(19.5f, 0.0f, -18.0f);
        glRotatef(-90, 0, 1, 0);
        glScalef(1.2f, 1.2f, 1.2f);
        glTranslatef(0.0f, 0.0f, 0.4f);

        drawFirePlane();
        glPopMatrix();

        // Cleanup
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glUseProgram(0);
    }

    // Draw ceiling shapes
    glPushMatrix();
    drawCeilingShapes();
    glPopMatrix();

    // Draw light position marker
    lighting_draw_debug_marker();
    glPopMatrix();
}

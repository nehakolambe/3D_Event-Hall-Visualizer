#include "CSCIx229.h"
#include <math.h>

// =======================================================
//                GLOBALS FOR OBJECT SYSTEM
// =======================================================
SceneObject objects[MAX_OBJECTS];
int objectCount = 0;
SceneObject* selectedObject = NULL;
int dragging = 0;

// =======================================================
//                INTERNAL UTILITIES
// =======================================================

// Utility to draw a colored quad
static void drawQuad(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float x3, float y3, float z3,
                     float x4, float y4, float z4,
                     float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

// Draw a wireframe bounding box for debugging
void drawBBox(SceneObject* o)
{
    glPushMatrix();
    glTranslatef(o->x, o->y, o->z);
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);

    float xmin = o->bbox[0], xmax = o->bbox[1];
    float ymin = o->bbox[2], ymax = o->bbox[3];
    float zmin = o->bbox[4], zmax = o->bbox[5];

    // bottom square
    glVertex3f(xmin, ymin, zmin); glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmin); glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmax); glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymin, zmax); glVertex3f(xmin, ymin, zmin);

    // top square
    glVertex3f(xmin, ymax, zmin); glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmin); glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmax); glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmax); glVertex3f(xmin, ymax, zmin);

    // vertical lines
    glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymin, zmin); glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymin, zmax); glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymin, zmax); glVertex3f(xmin, ymax, zmax);

    glEnd();
    glPopMatrix();
}

// Adds a new object to the scene
void addObject(const char* name, float x, float z, void (*drawFunc)(float, float), int movable)
{
    if (objectCount >= MAX_OBJECTS) return;

    SceneObject* obj = &objects[objectCount];
    obj->id = objectCount;
    strncpy(obj->name, name, sizeof(obj->name) - 1);
    obj->x = x;
    obj->y = 0;
    obj->z = z;
    obj->scale = 1.0;
    obj->rotation = 0;
    obj->drawFunc = drawFunc;
    obj->movable = movable;

    for (int i = 0; i < 6; i++) obj->bbox[i] = 0;
    objectCount++;
}

// =======================================================
//                SCENE INITIALIZATION
// =======================================================
void scene_init()
{
    objectCount = 0;

    // Chairs, tables, lamps — movable
    addObject("Table",          0,   0,  drawTable,         1);
    addObject("CocktailTable",  6,   0,  drawCocktailTable, 1);
    addObject("BanquetChair",  -4,   0,  drawBanquetChair,  1);
    addObject("Lamp",          -6,   0,  drawLamp,          1);

    // Door — non-movable
    addObject("Door",           0,  29.9, (void (*)(float,float))drawDoor, 0);

    // Curved screen — fixed
    addObject("CurvedScreen",   0, -30, (void (*)(float,float))drawCurvedScreen, 0);

    // Assign bounding boxes (approximate but generous)
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* o = &objects[i];
        if (strcmp(o->name, "Table") == 0)
            memcpy(o->bbox, (float[]){-3,3, 0,3, -3,3}, sizeof(o->bbox));
        else if (strcmp(o->name, "CocktailTable") == 0)
            memcpy(o->bbox, (float[]){-2,2, 0,4, -2,2}, sizeof(o->bbox));
        else if (strcmp(o->name, "BanquetChair") == 0)
            memcpy(o->bbox, (float[]){-1.5,1.5, 0,3, -1.5,1.5}, sizeof(o->bbox));
        else if (strcmp(o->name, "Lamp") == 0)
            memcpy(o->bbox, (float[]){-1,1, 0,6, -1,1}, sizeof(o->bbox));
        else
            memcpy(o->bbox, (float[]){-2,2, 0,3, -2,2}, sizeof(o->bbox));
    }
}

// =======================================================
//                SCENE RENDERING
// =======================================================
void scene_display()
{
    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    if (!debugMode)
    {
        // ==== Floor ====
        drawQuad(-20, 0, -30,  20, 0, -30,  20, 0, 30,  -20, 0, 30,  0.6, 0.6, 0.6);

        // ==== Ceiling ====
        drawQuad(-20, 15, -30,  20, 15, -30,  20, 15, 30,  -20, 15, 30,  0.9, 0.9, 0.9);

        // ==== Walls ====
        drawQuad(-20, 0, -30,  20, 0, -30,  20, 15, -30,  -20, 15, -30,  0.7, 0.7, 0.75);
        drawQuad(-20, 0, 30,  20, 0, 30,  20, 15, 30,  -20, 15, 30,  0.7, 0.7, 0.75);
        drawQuad(-20, 0, -30,  -20, 0, 30,  -20, 15, 30,  -20, 15, -30,  0.8, 0.8, 0.85);
        drawQuad(20, 0, -30,  20, 0, 30,  20, 15, 30,  20, 15, -30,  0.8, 0.8, 0.85);

        // ==== Stage ====
        float stageTop = 2.0;
        float stageBack = -30;
        float stageFront = stageBack + 10;
        float stageWidth = 10.0;

        drawQuad(-stageWidth, stageTop, stageBack,  stageWidth, stageTop, stageBack,
                 stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageFront,
                 0.4, 0.2, 0.1);
        drawQuad(-stageWidth, 0, stageFront,  stageWidth, 0, stageFront,
                 stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageFront,
                 0.35, 0.17, 0.09);
        drawQuad(-stageWidth, 0, stageBack,  stageWidth, 0, stageBack,
                 stageWidth, stageTop, stageBack, -stageWidth, stageTop, stageBack,
                 0.35, 0.17, 0.09);
        drawQuad(-stageWidth, 0, stageBack,  -stageWidth, 0, stageFront,
                 -stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageBack,
                 0.32, 0.16, 0.08);
        drawQuad(stageWidth, 0, stageFront,  stageWidth, 0, stageBack,
                 stageWidth, stageTop, stageBack,  stageWidth, stageTop, stageFront,
                 0.32, 0.16, 0.08);

        // ==== Draw all scene objects generically ====
        for (int i = 0; i < objectCount; i++)
        {
            SceneObject* obj = &objects[i];
            glPushMatrix();
            glTranslatef(obj->x, obj->y, obj->z);
            glRotatef(obj->rotation, 0, 1, 0);
            glScalef(obj->scale, obj->scale, obj->scale);

            if (strcmp(obj->name, "CurvedScreen") == 0)
                drawCurvedScreen(0, 0, 35.0, 10.0, 3.5, 25.0, 35.0, 0.5);
            else if (strcmp(obj->name, "Door") == 0)
                drawDoor(0.0, 0.0, 3.0, 7.0);
            else
                obj->drawFunc(0, 0);

            // Draw bounding boxes for debugging
            if (debugMode) drawBBox(obj);

            glPopMatrix();
        }

        // ==== Windows ====
        glColor3f(0.3, 0.5, 0.9);
        drawQuad(-19.9, 6, -15, -19.9, 6, -5, -19.9, 10, -5, -19.9, 10, -15, 0.3, 0.5, 0.9);
        drawQuad(-19.9, 6, 5, -19.9, 6, 15, -19.9, 10, 15, -19.9, 10, 5, 0.3, 0.5, 0.9);
        drawQuad(19.9, 6, -15, 19.9, 6, -5, 19.9, 10, -5, 19.9, 10, -15, 0.3, 0.5, 0.9);
        drawQuad(19.9, 6, 5, 19.9, 6, 15, 19.9, 10, 15, 19.9, 10, 5, 0.3, 0.5, 0.9);
    }
    else
    {
        // ==== Debug Mode ====
        glPushMatrix();
        switch (debugObjectIndex)
        {
        case 0: drawTable(0, 0); break;
        case 1: drawCocktailTable(0, 0); break;
        case 2: drawBanquetChair(0, 0); break;
        case 3: drawLamp(0, 0); break;
        case 4: drawCurvedScreen(0, 0, 35.0, 10.0, 0.0, 25.0, 35.0, 0.0); break;
        }
        glPopMatrix();
    }

    glPopMatrix();
}

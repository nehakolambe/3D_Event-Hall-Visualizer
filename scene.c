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

void drawBBox(SceneObject* o)
{
    glPushMatrix();
    glTranslatef(o->x, o->y, o->z);
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);

    float xmin = o->bbox[0], xmax = o->bbox[1];
    float ymin = o->bbox[2], ymax = o->bbox[3];
    float zmin = o->bbox[4], zmax = o->bbox[5];

    // bottom
    glVertex3f(xmin, ymin, zmin); glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmin); glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmax); glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymin, zmax); glVertex3f(xmin, ymin, zmin);

    // top
    glVertex3f(xmin, ymax, zmin); glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmin); glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmax); glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmax); glVertex3f(xmin, ymax, zmin);

    // verticals
    glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymin, zmin); glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymin, zmax); glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymin, zmax); glVertex3f(xmin, ymax, zmax);

    glEnd();
    glPopMatrix();
}

// =======================================================
//                SCENE INITIALIZATION
// =======================================================

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
    obj->solid = 1;

    for (int i = 0; i < 6; i++)
        obj->bbox[i] = 0;

    objectCount++;
}

void scene_init()
{
    objectCount = 0;

    // Movable furniture
    addObject("Table",          0,   0,  drawTable,         1);
    addObject("CocktailTable",  6,   0,  drawCocktailTable, 1);
    addObject("BanquetChair",  -4,   0,  drawBanquetChair,  1);
    addObject("Lamp",          -6,   0,  drawLamp,          1);

    // Fixed objects
    addObject("Door",          0,  29.9, (void (*)(float,float))drawDoor, 0);
    addObject("CurvedScreen",  0, -30,   (void (*)(float,float))drawCurvedScreen, 0);

    // Base heights
    objects[0].y = 0.0f;
    objects[1].y = 0.0f;
    objects[2].y = 0.0f;
    objects[3].y = 0.0f;

    // Assign bounding boxes
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* o = &objects[i];

        if (strcmp(o->name, "Table") == 0)
        {
            memcpy(o->bbox, (float[]){-1.2, 1.2, 0, 3, -0.6, 0.6}, sizeof(o->bbox));

            // Add leg colliders
            addObject("TableLeg_FL", o->x - 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_FR", o->x + 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_BL", o->x - 0.85f, o->z + 0.45f, NULL, 0);
            addObject("TableLeg_BR", o->x + 0.85f, o->z + 0.45f, NULL, 0);
        }
        else if (strcmp(o->name, "CocktailTable") == 0)
        {
            memcpy(o->bbox, (float[]){-0.7, 0.7, 0, 4, -0.7, 0.7}, sizeof(o->bbox));
        }
        else if (strcmp(o->name, "BanquetChair") == 0)
        {
            memcpy(o->bbox, (float[]){-0.5, 0.5, 0, 3, -0.5, 0.5}, sizeof(o->bbox));
        }
        else if (strcmp(o->name, "Lamp") == 0)
        {
            memcpy(o->bbox, (float[]){-0.5, 0.5, 0, 6, -0.5, 0.5}, sizeof(o->bbox));
        }
        else
        {
            memcpy(o->bbox, (float[]){-0.8, 0.8, 0, 3, -0.8, 0.8}, sizeof(o->bbox));
        }
    }

    // Leg colliders small bbox
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* o = &objects[i];
        if (strncmp(o->name, "TableLeg_", 9) == 0)
        {
            memcpy(o->bbox, (float[]){-0.1, 0.1, 0, 3, -0.1, 0.1}, sizeof(o->bbox));
            o->solid = 1;
        }
    }
}

// =======================================================
//                SCENE RENDERING
// =======================================================

void scene_display()
{
    glPushMatrix();
    lighting_update();
    glEnable(GL_LIGHTING);

    if (!debugMode)
    {
        // ==== Floor ====
        drawQuad(-20, 0, -30,  20, 0, -30,
                  20, 0, 30, -20, 0, 30,
                  0.6, 0.6, 0.6);

        // ==== Ceiling ====
        drawQuad(-20, 15, -30,  20, 15, -30,
                  20, 15, 30, -20, 15, 30,
                  0.9, 0.9, 0.9);

        // ==== Walls ====
        drawQuad(-20, 0, -30,  20, 0, -30,
                  20, 15, -30, -20, 15, -30,
                  0.7, 0.7, 0.75);

        drawQuad(-20, 0, 30,  20, 0, 30,
                  20, 15, 30, -20, 15, 30,
                  0.7, 0.7, 0.75);

        drawQuad(-20, 0, -30, -20, 0, 30,
                -20, 15, 30, -20, 15, -30,
                 0.8, 0.8, 0.85);

        drawQuad(20, 0, -30, 20, 0, 30,
                 20, 15, 30, 20, 15, -30,
                 0.8, 0.8, 0.85);

        // ==== Stage ====
        float stageTop = 2.0;
        float stageBack = -30;
        float stageFront = stageBack + 10;
        float stageWidth = 10.0;

        drawQuad(-stageWidth, stageTop, stageBack,
                  stageWidth, stageTop, stageBack,
                  stageWidth, stageTop, stageFront,
                 -stageWidth, stageTop, stageFront,
                 0.4, 0.2, 0.1);

        drawQuad(-stageWidth, 0, stageFront,
                  stageWidth, 0, stageFront,
                  stageWidth, stageTop, stageFront,
                 -stageWidth, stageTop, stageFront,
                 0.35, 0.17, 0.09);

        // ==== Objects ====
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
                drawDoor(0, 0, 3.0, 7.0);
            else if (obj->drawFunc)
                obj->drawFunc(0, 0);

            if (debugMode)
                drawBBox(obj);
            glPopMatrix();
        }

        // ==== Windows ====
        glColor3f(0.3, 0.5, 0.9);
        drawQuad(-19.9, 6, -15, -19.9, 6, -5,
                 -19.9, 10, -5, -19.9, 10, -15,
                  0.3, 0.5, 0.9);

        drawQuad(-19.9, 6, 5, -19.9, 6, 15,
                 -19.9, 10, 15, -19.9, 10, 5,
                  0.3, 0.5, 0.9);

        drawQuad(19.9, 6, -15, 19.9, 6, -5,
                 19.9, 10, -5, 19.9, 10, -15,
                 0.3, 0.5, 0.9);

        drawQuad(19.9, 6, 5, 19.9, 6, 15,
                 19.9, 10, 15, 19.9, 10, 5,
                 0.3, 0.5, 0.9);
    }
    else
    {
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
    lighting_draw_debug_marker();
    glPopMatrix();
}

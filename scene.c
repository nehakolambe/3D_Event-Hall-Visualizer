#include "CSCIx229.h"
#include <math.h>

// =======================================================
//                GLOBALS FOR OBJECT SYSTEM
// =======================================================
SceneObject objects[MAX_OBJECTS];
int objectCount = 0;
SceneObject* selectedObject = NULL;
int dragging = 0;

unsigned int wallTex;   // Texture ID for wall texture
unsigned int floorTex;  // Texture ID for floor texture
unsigned int screenTex;  // Texture ID for screen texture

// void placeEventTables();
// void placeCocktailTables();
// void placeDoorLamps();

// =======================================================
//                INTERNAL UTILITIES
// =======================================================

static void drawQuadN(float x1,float y1,float z1,
                      float x2,float y2,float z2,
                      float x3,float y3,float z3,
                      float x4,float y4,float z4,
                      float nx,float ny,float nz,
                      float r,float g,float b)
{
    GLboolean texEnabled;
    glGetBooleanv(GL_TEXTURE_2D, &texEnabled);

    // if no texture bound → keep your color shading
    if (!texEnabled)
        glColor3f(r, g, b);

    glNormal3f(nx, ny, nz);
    glBegin(GL_QUADS);

    if (texEnabled)
    {
        glTexCoord2f(0,0);  glVertex3f(x1,y1,z1);
        glTexCoord2f(1,0);  glVertex3f(x2,y2,z2);
        glTexCoord2f(1,1);  glVertex3f(x3,y3,z3);
        glTexCoord2f(0,1);  glVertex3f(x4,y4,z4);
    }
    else
    {
        glVertex3f(x1,y1,z1);
        glVertex3f(x2,y2,z2);
        glVertex3f(x3,y3,z3);
        glVertex3f(x4,y4,z4);
    }

    glEnd();
}

// =======================================================
//    Reusable tiled surface drawer
//    Draws a surface subdivided into many 2×2 tiles
// =======================================================
//
//  xmin,xmax   → width direction (horizontal)
//  ymin,ymax   → height direction (vertical)
//  zmin,zmax   → depth direction
//
//  nx,ny,nz    → normal for entire surface
//
//  dir         → plane selector
//                0 = XZ plane (floor/ceiling)
//                1 = XY plane (front/back walls)
//                2 = ZY plane (left/right walls)
//
//  tileSize    → recommended = 2.0
//
void drawTiledSurface(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float nx, float ny, float nz,
    float tileSize
)
{
    // Determine orientation
    int dir;

    if (y1 == y2)      dir = 0; // XZ plane (floor, ceiling)
    else if (z1 == z2) dir = 1; // XY plane (front/back)
    else               dir = 2; // ZY plane (left/right)

    glNormal3f(nx, ny, nz);

    // Floor/Ceiling (XZ)
    if (dir == 0)
    {
        float xmin = x1, xmax = x2;
        float zmin = z1, zmax = z2;
        float y = y1;

        for (float x = xmin; x < xmax; x += tileSize)
        {
            for (float z = zmin; z < zmax; z += tileSize)
            {
                float xA = x;
                float xB = fmin(x + tileSize, xmax);
                float zA = z;
                float zB = fmin(z + tileSize, zmax);

                glBegin(GL_QUADS);
                    glTexCoord2f(0,0); glVertex3f(xA, y, zA);
                    glTexCoord2f(1,0); glVertex3f(xB, y, zA);
                    glTexCoord2f(1,1); glVertex3f(xB, y, zB);
                    glTexCoord2f(0,1); glVertex3f(xA, y, zB);
                glEnd();
            }
        }
    }

    // Front/Back walls (XY)
    else if (dir == 1)
    {
        float xmin = x1, xmax = x2;
        float ymin = y1, ymax = y2;
        float z = z1;

        for (float x = xmin; x < xmax; x += tileSize)
        {
            for (float y = ymin; y < ymax; y += tileSize)
            {
                float xA = x;
                float xB = fmin(x + tileSize, xmax);
                float yA = y;
                float yB = fmin(y + tileSize, ymax);

                glBegin(GL_QUADS);
                    glTexCoord2f(0,0); glVertex3f(xA, yA, z);
                    glTexCoord2f(1,0); glVertex3f(xB, yA, z);
                    glTexCoord2f(1,1); glVertex3f(xB, yB, z);
                    glTexCoord2f(0,1); glVertex3f(xA, yB, z);
                glEnd();
            }
        }
    }

    // Left/Right walls (ZY)
    else
    {
        float zmin = z1, zmax = z2;
        float ymin = y1, ymax = y2;
        float x = x1;

        for (float z = zmin; z < zmax; z += tileSize)
        {
            for (float y = ymin; y < ymax; y += tileSize)
            {
                float zA = z;
                float zB = fmin(z + tileSize, zmax);
                float yA = y;
                float yB = fmin(y + tileSize, ymax);

                glBegin(GL_QUADS);
                    glTexCoord2f(0,0); glVertex3f(x, yA, zA);
                    glTexCoord2f(1,0); glVertex3f(x, yA, zB);
                    glTexCoord2f(1,1); glVertex3f(x, yB, zB);
                    glTexCoord2f(0,1); glVertex3f(x, yB, zA);
                glEnd();
            }
        }
    }
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
    wallTex = LoadTexBMP("textures/wall.bmp");
    floorTex = LoadTexBMP("textures/carpet.bmp");
    screenTex = LoadTexBMP("textures/screen.bmp");
    objectCount = 0;

    // =======================================================
    // Base movable prototypes (for user to drag)
    // =======================================================
    // addObject("Table",          0,   0,  drawTable,         1);
    // addObject("CocktailTable",  6,   0,  drawCocktailTable, 1);
    // addObject("BanquetChair",  -4,   0,  drawBanquetChair,  1);
    // addObject("Lamp",          -6,   0,  drawLamp,          1);

    // =======================================================
    // Fixed objects
    // =======================================================
    addObject("Door",          0,  29.9, (void (*)(float,float))drawDoor, 0);
    addObject("CurvedScreen",  0, -30,   (void (*)(float,float))drawCurvedScreen, 0);

    // Base heights (only if needed)
    objects[0].y = 0.0f;
    objects[1].y = 0.0f;
    objects[2].y = 0.0f;
    objects[3].y = 0.0f;

    // =======================================================
    // EVENT LAYOUT OBJECTS (ALL MOVABLE)
    // =======================================================

    // ---------------------------
    // 4 rectangular tables
    // ---------------------------
    float tableX[4] = {-10, -10,  10, 10};
    float tableZ[4] = {-10,   0, -10,  0};

    int tableIndexStart = objectCount;

    for (int i = 0; i < 4; i++)
    {
        char name[32];
        sprintf(name, "EventTable%d", i+1);
        addObject(name, tableX[i], tableZ[i], drawTable, 1);
    }

    // ---------------------------
    // 8 chairs (2 per table)
    // ---------------------------
    float chairOffsetX[2] = {0.0, 0.0};
    float chairOffsetZ[2] = {-1.8, 1.8};
    float chairRot[2]     = {  0 , 180};

    for (int t = 0; t < 4; t++)
    {
        for (int c = 0; c < 2; c++)
        {
            char cname[32];
            sprintf(cname, "EventChair_T%d_C%d", t+1, c+1);

            float cx = tableX[t] + chairOffsetX[c];
            float cz = tableZ[t] + chairOffsetZ[c];

            addObject(cname, cx, cz, drawBanquetChair, 1);

            // rotate chair towards table
            objects[objectCount - 1].rotation = chairRot[c];
        }
    }

    // ---------------------------
    // 3 cocktail tables
    // ---------------------------
    float ctX[3] = {0, -12, 12};
    float ctZ[3] = {-5, -3, -3};

    for (int i = 0; i < 3; i++)
    {
        char name[32];
        sprintf(name, "Cocktail_%d", i+1);
        addObject(name, ctX[i], ctZ[i], drawCocktailTable, 1);
    }

    // ---------------------------
    // 2 door lamps
    // ---------------------------
    float lampX[2] = {-4, 4};
    float lampZ = 28.5;

    addObject("DoorLamp_1", -3, 22, drawLamp, 1);

    // =======================================================
    // ASSIGN BOUNDING BOXES (including newly added objects)
    // =======================================================
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* o = &objects[i];

        if (strcmp(o->name, "Table") == 0 ||
            strncmp(o->name, "EventTable", 10) == 0)
        {
            memcpy(o->bbox, (float[]){-1.2, 1.2, 0, 3, -0.6, 0.6}, sizeof(o->bbox));

            // Add leg colliders
            addObject("TableLeg_FL", o->x - 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_FR", o->x + 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_BL", o->x - 0.85f, o->z + 0.45f, NULL, 0);
            addObject("TableLeg_BR", o->x + 0.85f, o->z + 0.45f, NULL, 0);
        }
        else if (strcmp(o->name, "CocktailTable") == 0 ||
                 strncmp(o->name, "Cocktail_", 9) == 0)
        {
            memcpy(o->bbox, (float[]){-0.7, 0.7, 0, 4, -0.7, 0.7}, sizeof(o->bbox));
        }
        else if (strcmp(o->name, "BanquetChair") == 0 ||
                 strncmp(o->name, "EventChair", 10) == 0)
        {
            memcpy(o->bbox, (float[]){-0.5, 0.5, 0, 3, -0.5, 0.5}, sizeof(o->bbox));
        }
        else if (strcmp(o->name, "Lamp") == 0 ||
                 strncmp(o->name, "DoorLamp", 8) == 0)
        {
            memcpy(o->bbox, (float[]){-0.5, 0.5, 0, 6, -0.5, 0.5}, sizeof(o->bbox));
        }
        else
        {
            memcpy(o->bbox, (float[]){-0.8, 0.8, 0, 3, -0.8, 0.8}, sizeof(o->bbox));
        }
    }

    // =======================================================
    // Fix table leg collider bbox
    // =======================================================
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
        // ==== Floor (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, floorTex);
drawTiledSurface(-20,0,-30, 20,0,30, 0,1,0, 2.0);
glDisable(GL_TEXTURE_2D);

// ==== Ceiling (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, wallTex);
drawTiledSurface(-20,15,-30, 20,15,30, 0,-1,0, 2.0);
glDisable(GL_TEXTURE_2D);

// ==== Back Wall (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, wallTex);
drawTiledSurface(-20,0,-30, 20,15,-30, 0,0,1, 2.0);
glDisable(GL_TEXTURE_2D);

// ==== Front Wall (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, wallTex);
drawTiledSurface(-20,0,30, 20,15,30, 0,0,-1, 2.0);
glDisable(GL_TEXTURE_2D);

// ==== Left Wall (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, wallTex);
drawTiledSurface(-20,0,-30, -20,15,30, 1,0,0, 2.0);
glDisable(GL_TEXTURE_2D);

// ==== Right Wall (tiled) ====
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, wallTex);
drawTiledSurface(20,0,-30, 20,15,30, -1,0,0, 2.0);
glDisable(GL_TEXTURE_2D);


        // ==== Stage ====
        float stageTop = 2.0;
        float stageBack = -30;
        float stageFront = stageBack + 10;
        float stageWidth = 10.0;

        drawQuadN(-stageWidth,stageTop,stageBack,
          stageWidth,stageTop,stageBack,
          stageWidth,stageTop,stageFront,
          -stageWidth,stageTop,stageFront,
          0,1,0,
          0.4,0.2,0.1);

        drawQuadN(-stageWidth,0,stageFront,
          stageWidth,0,stageFront,
          stageWidth,stageTop,stageFront,
          -stageWidth,stageTop,stageFront,
          0,0,-1,
          0.35,0.17,0.09);

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
            {
                if (debugMode && strcmp(obj->name, "BanquetChair") == 0)
                {
                    glPushMatrix();
                    glScalef(1.8, 1.8, 1.8);   // Make chair bigger in debug mode
                    obj->drawFunc(0, 0);
                    glPopMatrix();
                }
                else
                {
                    obj->drawFunc(0, 0);
                }
            }

            if (debugMode)
                drawBBox(obj);
            glPopMatrix();
        }

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

// void placeEventTables()
// {
//     // Table positions (4 tables)
//     float tx[] = {-10, -10, 10, 10};
//     float tz[] = {-10,  0, -10,  0};

//     // 2 chairs per table, placed on opposite sides
//     float cx[] = {0.0, 0.0};      // X offset for chairs
//     float cz[] = {-1.8, 1.8};     // Z offset for chairs (front/back)

//     for (int t = 0; t < 4; t++)
//     {
//         // Draw the table
//         glPushMatrix();
//         glTranslatef(tx[t], 0, tz[t]);
//         drawTable(0, 0);
//         glPopMatrix();

//         // Draw 2 chairs per table
//         for (int c = 0; c < 2; c++)
//         {
//             glPushMatrix();
//             glTranslatef(tx[t] + cx[c], 0, tz[t] + cz[c]);

//             // Chair faces the table
//             if (c == 0) glRotatef(0, 0, 1, 0);     // front chair
//             if (c == 1) glRotatef(180, 0, 1, 0);   // back chair

//             drawBanquetChair(0, 0);
//             glPopMatrix();
//         }
//     }
// }



// void placeCocktailTables()
// {
//     float x[] = {0, -12, 12};
//     float z[] = {-5, -3, -3};

//     for (int i = 0; i < 3; i++)
//     {
//         glPushMatrix();
//         glTranslatef(x[i], 0, z[i]);
//         drawCocktailTable(0, 0);
//         glPopMatrix();
//     }
// }


// void placeDoorLamps()
// {
//     // Lamps left & right of door
//     float x[] = {-4, 4};
//     float z = 28.5;

//     for (int i = 0; i < 2; i++)
//     {
//         glPushMatrix();
//         glTranslatef(x[i], 0, z);
//         drawLamp(0, 0);
//         glPopMatrix();
//     }
// }

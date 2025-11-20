#include "CSCIx229.h"

// Object system globals
SceneObject objects[MAX_OBJECTS];
int objectCount = 0;
SceneObject *selectedObject = NULL;
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

// Quad
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

// Draw a tiled surface for floors/walls with texture repeat
void drawTiledSurface(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float nx, float ny, float nz,
    float tileSize)
{
    int dir;

    if (y1 == y2)
        dir = 0; // XZ plane
    else if (z1 == z2)
        dir = 1; // XY plane
    else
        dir = 2; // ZY plane

    glNormal3f(nx, ny, nz);

    // XZ plane (floor/ceiling)
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
                float xB = fminf(x + tileSize, xmax);
                float zA = z;
                float zB = fminf(z + tileSize, zmax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(xA, y, zA);
                glTexCoord2f(1, 0);
                glVertex3f(xB, y, zA);
                glTexCoord2f(1, 1);
                glVertex3f(xB, y, zB);
                glTexCoord2f(0, 1);
                glVertex3f(xA, y, zB);
                glEnd();
            }
        }
    }

    // XY plane (front/back walls)
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
                float xB = fminf(x + tileSize, xmax);
                float yA = y;
                float yB = fminf(y + tileSize, ymax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(xA, yA, z);
                glTexCoord2f(1, 0);
                glVertex3f(xB, yA, z);
                glTexCoord2f(1, 1);
                glVertex3f(xB, yB, z);
                glTexCoord2f(0, 1);
                glVertex3f(xA, yB, z);
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

        for (float z = zmin; z < zmax; z += tileSize)
        {
            for (float y = ymin; y < ymax; y += tileSize)
            {
                float zA = z;
                float zB = fminf(z + tileSize, zmax);
                float yA = y;
                float yB = fminf(y + tileSize, ymax);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(x, yA, zA);
                glTexCoord2f(1, 0);
                glVertex3f(x, yA, zB);
                glTexCoord2f(1, 1);
                glVertex3f(x, yB, zB);
                glTexCoord2f(0, 1);
                glVertex3f(x, yB, zA);
                glEnd();
            }
        }
    }
}

// draws a simple bounding box
void drawBBox(SceneObject *o)
{
    glPushMatrix();
    glTranslatef(o->x, o->y, o->z);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);

    float xmin = o->bbox[0], xmax = o->bbox[1];
    float ymin = o->bbox[2], ymax = o->bbox[3];
    float zmin = o->bbox[4], zmax = o->bbox[5];

    // bottom
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymin, zmin);

    // top
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmin, ymax, zmin);

    // verticals
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymax, zmax);

    glEnd();
    glPopMatrix();
}

// creates a new scene object
void addObject(const char *name,
               float x, float z,
               void (*drawFunc)(float, float),
               int movable)
{
    if (objectCount >= MAX_OBJECTS)
        return;

    SceneObject *obj = &objects[objectCount];

    obj->id = objectCount;

    // safe copy
    strncpy(obj->name, name, sizeof(obj->name) - 1);
    obj->name[sizeof(obj->name) - 1] = '\0';

    obj->x = x;
    obj->y = 0.0f;
    obj->z = z;

    obj->scale = 1.0f;
    obj->rotation = 0.0f;
    obj->drawFunc = drawFunc;
    obj->movable = movable;
    obj->solid = 1;

    // reset bbox
    for (int i = 0; i < 6; i++)
        obj->bbox[i] = 0.0f;

    objectCount++;
}

void scene_init()
{
    // load textures
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

    objectCount = 0;

    // fixed objects
    addObject("Door", 0.0f, 29.9f, (void (*)(float, float))drawDoor, 0);
    addObject("CurvedScreen", 0.0f, -30.0f, (void (*)(float, float))drawCurvedScreen, 0);

    objects[0].y = 0.0f;
    objects[1].y = 0.0f;

    // event tables
    float tableX[4] = {-10, -10, 10, 10};
    float tableZ[4] = {-10, 0, -10, 0};

    for (int i = 0; i < 4; i++)
    {
        char name[32];
        sprintf(name, "EventTable%d", i + 1);
        addObject(name, tableX[i], tableZ[i], drawTable, 1);
    }

    // event chairs
    float chairOffsetX[2] = {0.0f, 0.0f};
    float chairOffsetZ[2] = {-1.8f, 1.8f};
    float chairRot[2] = {0.0f, 180.0f};

    for (int t = 0; t < 4; t++)
    {
        for (int c = 0; c < 2; c++)
        {
            char cname[32];
            sprintf(cname, "EventChair_T%d_C%d", t + 1, c + 1);

            float cx = tableX[t] + chairOffsetX[c];
            float cz = tableZ[t] + chairOffsetZ[c];

            addObject(cname, cx, cz, drawBanquetChair, 1);
            objects[objectCount - 1].rotation = chairRot[c];
        }
    }

    // cocktail tables
    float ctX[3] = {0, -12, 12};
    float ctZ[3] = {-5, -3, -3};

    addObject("Cocktail_1", ctX[0], ctZ[0], drawCocktailTable, 1);
    addObject("Cocktail_2", ctX[1], ctZ[1] + 10.0f, drawCocktailTable2, 1);
    addObject("Cocktail_3", ctX[2], ctZ[2] + 10.0f, drawCocktailTable3, 1);

    // bar chairs
    addObject("BarChair_3",
              ctX[2] + 1.25f,
              ctZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 200.0f;

    addObject("BarChair_4",
              ctX[2] - 1.25f,
              ctZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 160.0f;

    // door lamp
    addObject("DoorLamp_1", -5.0f, 11.0f, drawLamp, 1);

    // meeting table
    float meetX = 0.0f;
    float meetZ = 11.0f;

    addObject("MeetingTable", meetX, meetZ, drawMeetingTable, 1);

    // meeting chairs
    float mChairX[2] = {-1.8f, 1.8f};
    float mChairZfront = -2.2f;
    float mChairZback = 2.2f;

    // front chairs
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_F%d", i + 1);

        addObject(cname,
                  meetX + mChairX[i],
                  meetZ + mChairZfront,
                  drawBanquetChair,
                  1);

        objects[objectCount - 1].rotation = 0.0f;
    }

    // back chairs
    for (int i = 0; i < 2; i++)
    {
        char cname[32];
        sprintf(cname, "MeetChair_B%d", i + 1);

        addObject(cname,
                  meetX + mChairX[i],
                  meetZ + mChairZback,
                  drawBanquetChair,
                  1);

        objects[objectCount - 1].rotation = 180.0f;
    }

    // assign bounding boxes
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *o = &objects[i];

        if (strcmp(o->name, "Table") == 0 ||
            strncmp(o->name, "EventTable", 10) == 0)
        {
            memcpy(o->bbox, ((float[]){-1.2f, 1.2f, 0, 3, -0.6f, 0.6f}), sizeof(o->bbox));

            addObject("TableLeg_FL", o->x - 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_FR", o->x + 0.85f, o->z - 0.45f, NULL, 0);
            addObject("TableLeg_BL", o->x - 0.85f, o->z + 0.45f, NULL, 0);
            addObject("TableLeg_BR", o->x + 0.85f, o->z + 0.45f, NULL, 0);
        }
        else if (strncmp(o->name, "Cocktail", 8) == 0)
        {
            memcpy(o->bbox, ((float[]){-0.7f, 0.7f, 0, 4, -0.7f, 0.7f}), sizeof(o->bbox));
        }
        else if (strncmp(o->name, "EventChair", 10) == 0 ||
                 strncmp(o->name, "BanquetChair", 12) == 0 ||
                 strncmp(o->name, "MeetChair", 9) == 0)
        {
            memcpy(o->bbox, ((float[]){-0.5f, 0.5f, 0, 3, -0.5f, 0.5f}), sizeof(o->bbox));
        }
        else if (strncmp(o->name, "DoorLamp", 8) == 0 ||
                 strcmp(o->name, "Lamp") == 0)
        {
            memcpy(o->bbox, ((float[]){-0.5f, 0.5f, 0, 6, -0.5f, 0.5f}), sizeof(o->bbox));
        }
        else if (strncmp(o->name, "BarChair", 8) == 0)
        {
            memcpy(o->bbox, ((float[]){-0.4f, 0.4f, 0, 4.2f, -0.4f, 0.4f}), sizeof(o->bbox));
        }
        else if (strcmp(o->name, "MeetingTable") == 0)
        {
            memcpy(o->bbox, ((float[]){-4.0f, 4.0f, 0, 2.2f, -1.5f, 1.5f}), sizeof(o->bbox));
        }
        else
        {
            memcpy(o->bbox, ((float[]){-0.8f, 0.8f, 0, 3, -0.8f, 0.8f}), sizeof(o->bbox));
        }
    }

    // table leg collider bounding box
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *o = &objects[i];

        if (strncmp(o->name, "TableLeg_", 9) == 0)
        {
            memcpy(o->bbox, ((float[]){-0.1f, 0.1f, 0, 3, -0.1f, 0.1f}), sizeof(o->bbox));
            o->solid = 1;
        }
    }
}

// Scene Rendering
void scene_display()
{
    glPushMatrix();
    lighting_update();
    glEnable(GL_LIGHTING);

    // Floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTex);
    drawTiledSurface(-20, 0, -30, 20, 0, 30, 0, 1, 0, 2.0);
    glDisable(GL_TEXTURE_2D);

    // Ceiling
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    drawTiledSurface(-20, 15, -30, 20, 15, 30, 0, -1, 0, 2.0);
    glDisable(GL_TEXTURE_2D);

    // Back wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    drawTiledSurface(-20, 0, -30, 20, 15, -30, 0, 0, 1, 2.0);
    glDisable(GL_TEXTURE_2D);

    // Front wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    drawTiledSurface(-20, 0, 30, 20, 15, 30, 0, 0, -1, 2.0);
    glDisable(GL_TEXTURE_2D);

    // Left wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    drawTiledSurface(-20, 0, -30, -20, 15, 30, 1, 0, 0, 2.0);
    glDisable(GL_TEXTURE_2D);

    // Right wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTex);
    drawTiledSurface(20, 0, -30, 20, 15, 30, -1, 0, 0, 2.0);
    glDisable(GL_TEXTURE_2D);

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
        SceneObject *obj = &objects[i];

        glPushMatrix();
        glTranslatef(obj->x, obj->y, obj->z);
        glRotatef(obj->rotation, 0, 1, 0);
        glScalef(obj->scale, obj->scale, obj->scale);

        if (strcmp(obj->name, "CurvedScreen") == 0)
        {
            drawCurvedScreen(0, 0, 35.0, 10.0, 3.5, 25.0, 35.0, 0.5);
        }
        else if (strcmp(obj->name, "Door") == 0)
        {
            drawDoor(0, 0, 3.0, 7.0);
        }
        else if (obj->drawFunc)
        {
            obj->drawFunc(0, 0);
        }

        glPopMatrix();
    }
    // Ceiling lights
    glPushMatrix();
    drawCeilingLights();
    glPopMatrix();

    lighting_draw_debug_marker();
    glPopMatrix();
}
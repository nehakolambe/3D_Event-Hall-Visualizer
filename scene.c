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

typedef struct
{
    const char *baseName;
    void (*drawFunc)(float, float);
    int movable;
    float defaultRotation;
    float defaultScale;
} ObjectTemplate;

static const ObjectTemplate spawnTemplates[SPAWN_TYPE_COUNT] = {
    [SPAWN_LAMP] = {"Lamp", drawLamp, 1, 0.0f, 1.0f},
    [SPAWN_EVENT_TABLE] = {"EventTable", drawTable, 1, 0.0f, 1.0f},
    [SPAWN_MEETING_TABLE] = {"MeetingTable", drawMeetingTable, 1, 0.0f, 1.0f},
    [SPAWN_BAR_CHAIR] = {"BarChair", drawBarChairObj, 1, 0.0f, 1.0f},
    [SPAWN_BANQUET_CHAIR] = {"BanquetChair", drawBanquetChair, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_1] = {"Cocktail_1", drawCocktailTable, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_2] = {"Cocktail_2", drawCocktailTable2, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_3] = {"Cocktail_3", drawCocktailTable3, 1, 0.0f, 1.0f}
};

static int spawnCounters[SPAWN_TYPE_COUNT] = {0};

static void configureObjectBounds(SceneObject *obj);
static int findFreeGroundSpot(SceneObject *prototype, float *outX, float *outZ);
static int nameHasPrefix(const char *name, const char *prefix);

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

// Simple flat whiteboard on the left wall
static void drawWhiteboardTrigger(float x, float z)
{
    (void)x;
    (void)z;

    const float halfWidth = 3.0f;
    const float boardBottom = 2.0f;
    const float boardTop = 6.0f;

    const float boardLeft = -halfWidth;
    const float boardRight = halfWidth;
    const int boardRows = 24;
    const int boardCols = 48;

    GLboolean wasLighting = glIsEnabled(GL_LIGHTING);
    GLboolean wasTexturing = glIsEnabled(GL_TEXTURE_2D);
    GLint previousShadeModel;
    glGetIntegerv(GL_SHADE_MODEL, &previousShadeModel);

    if (!wasLighting)
        glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    const GLfloat boardAmbient[] = {0.12f, 0.12f, 0.12f, 1.0f};
    const GLfloat boardDiffuse[] = {0.92f, 0.92f, 0.92f, 1.0f};
    const GLfloat boardSpecular[] = {0.9f, 0.9f, 0.9f, 1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, boardAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, boardDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, boardSpecular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 64);

    float yStep = (boardTop - boardBottom) / (float)boardRows;
    float zStep = (boardRight - boardLeft) / (float)boardCols;

    for (int r = 0; r < boardRows; r++)
    {
        float y0 = boardBottom + r * yStep;
        float y1 = y0 + yStep;
        for (int c = 0; c < boardCols; c++)
        {
            float z0 = boardLeft + c * zStep;
            float z1 = z0 + zStep;

            glBegin(GL_QUADS);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, y0, z0);
            glVertex3f(0.0f, y0, z1);
            glVertex3f(0.0f, y1, z1);
            glVertex3f(0.0f, y1, z0);
            glEnd();
        }
    }

    if (previousShadeModel != GL_SMOOTH)
        glShadeModel(previousShadeModel);
    if (wasTexturing)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
    if (!wasLighting)
        glDisable(GL_LIGHTING);

    GLboolean borderLighting = glIsEnabled(GL_LIGHTING);
    if (borderLighting)
        glDisable(GL_LIGHTING);

    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, boardBottom, boardLeft);
    glVertex3f(0.0f, boardBottom, boardRight);
    glVertex3f(0.0f, boardTop, boardRight);
    glVertex3f(0.0f, boardTop, boardLeft);
    glEnd();
    glLineWidth(1.0f);

    if (borderLighting)
        glEnable(GL_LIGHTING);

    whiteboard_render_on_board(boardBottom, boardTop, boardLeft, boardRight);
}

// draws a simple bounding box
void drawBBox(SceneObject *o)
{
    glPushMatrix();
    glTranslatef(o->x, o->y, o->z);
    glRotatef(o->rotation, 0, 1, 0);
    glColor3f(1.0f, 0.0f, 0.0f); // red wireframe

    for (int b = 0; b < o->subBoxCount; b++)
    {
        float xmin = o->subBox[b][0];
        float xmax = o->subBox[b][1];
        float ymin = o->subBox[b][2];
        float ymax = o->subBox[b][3];
        float zmin = o->subBox[b][4];
        float zmax = o->subBox[b][5];

        glBegin(GL_LINES);

        // bottom rectangle
        glVertex3f(xmin, ymin, zmin);
        glVertex3f(xmax, ymin, zmin);
        glVertex3f(xmax, ymin, zmin);
        glVertex3f(xmax, ymin, zmax);
        glVertex3f(xmax, ymin, zmax);
        glVertex3f(xmin, ymin, zmax);
        glVertex3f(xmin, ymin, zmax);
        glVertex3f(xmin, ymin, zmin);

        // top rectangle
        glVertex3f(xmin, ymax, zmin);
        glVertex3f(xmax, ymax, zmin);
        glVertex3f(xmax, ymax, zmin);
        glVertex3f(xmax, ymax, zmax);
        glVertex3f(xmax, ymax, zmax);
        glVertex3f(xmin, ymax, zmax);
        glVertex3f(xmin, ymax, zmax);
        glVertex3f(xmin, ymax, zmin);

        // vertical lines
        glVertex3f(xmin, ymin, zmin);
        glVertex3f(xmin, ymax, zmin);
        glVertex3f(xmax, ymin, zmin);
        glVertex3f(xmax, ymax, zmin);
        glVertex3f(xmax, ymin, zmax);
        glVertex3f(xmax, ymax, zmax);
        glVertex3f(xmin, ymin, zmax);
        glVertex3f(xmin, ymax, zmax);

        glEnd();
    }

    glPopMatrix();
}

// creates a new scene object
SceneObject *addObject(const char *name,
                       float x, float z,
                       void (*drawFunc)(float, float),
                       int movable)
{
    if (objectCount >= MAX_OBJECTS)
        return NULL;

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
    obj->subBoxCount = 0;
    for (int k = 0; k < MAX_SUBBOXES; k++)
        for (int j = 0; j < 6; j++)
            obj->subBox[k][j] = 0.0f;

    objectCount++;
    return obj;
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
    memset(spawnCounters, 0, sizeof(spawnCounters));

    // fixed objects
    addObject("Door", 0.0f, 29.9f, (void (*)(float, float))drawDoor, 0);
    addObject("CurvedScreen", 0.0f, -30.0f, (void (*)(float, float))drawCurvedScreen, 0);

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
    addObject("BarChair_1",
              ctX[2] + 1.25f,
              ctZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 200.0f;

    addObject("BarChair_2",
              ctX[2] - 1.25f,
              ctZ[2] + 11.8f,
              drawBarChairObj,
              1);
    objects[objectCount - 1].rotation = 160.0f;

    // door lamp
    addObject("Lamp", -5.0f, 11.0f, drawLamp, 1);

    // whiteboard trigger on left wall
    addObject("Whiteboard", -19.5f, -5.0f, drawWhiteboardTrigger, 0);

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

    for (int i = 0; i < objectCount; i++)
        configureObjectBounds(&objects[i]);
}

static int nameHasPrefix(const char *name, const char *prefix)
{
    if (!name || !prefix)
        return 0;

    size_t len = strlen(prefix);
    return strncmp(name, prefix, len) == 0;
}

static void configureObjectBounds(SceneObject *obj)
{
    if (!obj)
        return;

    if (nameHasPrefix(obj->name, "EventTable"))
    {
        obj->subBoxCount = 2;

        obj->subBox[0][0] = -2.15f;
        obj->subBox[0][1] = 2.15f;
        obj->subBox[0][2] = 0.0f;
        obj->subBox[0][3] = 2.8f;
        obj->subBox[0][4] = -1.45f;
        obj->subBox[0][5] = 1.45f;

        obj->subBox[1][0] = -2.05f;
        obj->subBox[1][1] = 2.05f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 2.2f;
        obj->subBox[1][4] = -1.05f;
        obj->subBox[1][5] = 1.05f;
        return;
    }

    if (nameHasPrefix(obj->name, "Cocktail_1"))
    {
        obj->subBoxCount = 3;

        obj->subBox[0][0] = -1.6f;
        obj->subBox[0][1] = 1.6f;
        obj->subBox[0][2] = 4.0f;
        obj->subBox[0][3] = 4.2f;
        obj->subBox[0][4] = -1.6f;
        obj->subBox[0][5] = 1.6f;

        obj->subBox[1][0] = -0.25f;
        obj->subBox[1][1] = 0.25f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 4.0f;
        obj->subBox[1][4] = -0.25f;
        obj->subBox[1][5] = 0.25f;

        obj->subBox[2][0] = -0.7f;
        obj->subBox[2][1] = 0.7f;
        obj->subBox[2][2] = 0.0f;
        obj->subBox[2][3] = 0.15f;
        obj->subBox[2][4] = -0.7f;
        obj->subBox[2][5] = 0.7f;
        return;
    }

    if (nameHasPrefix(obj->name, "Cocktail_2"))
    {
        obj->subBoxCount = 2;

        obj->subBox[0][0] = -1.5f;
        obj->subBox[0][1] = 1.5f;
        obj->subBox[0][2] = 4.0f;
        obj->subBox[0][3] = 4.25f;
        obj->subBox[0][4] = -1.5f;
        obj->subBox[0][5] = 1.5f;

        obj->subBox[1][0] = -1.0f;
        obj->subBox[1][1] = 1.0f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 4.0f;
        obj->subBox[1][4] = -1.0f;
        obj->subBox[1][5] = 1.0f;
        return;
    }

    if (nameHasPrefix(obj->name, "Cocktail_3"))
    {
        obj->subBoxCount = 2;

        obj->subBox[0][0] = -1.05f;
        obj->subBox[0][1] = 1.05f;
        obj->subBox[0][2] = 2.3f;
        obj->subBox[0][3] = 4.00f;
        obj->subBox[0][4] = -1.05f;
        obj->subBox[0][5] = 1.05f;

        obj->subBox[1][0] = -1.0f;
        obj->subBox[1][1] = 1.0f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 2.3f;
        obj->subBox[1][4] = -1.0f;
        obj->subBox[1][5] = 1.0f;
        return;
    }

    if (nameHasPrefix(obj->name, "EventChair") ||
        nameHasPrefix(obj->name, "MeetChair") ||
        nameHasPrefix(obj->name, "BanquetChair"))
    {
        obj->subBoxCount = 3;

        obj->subBox[0][0] = -0.6f;
        obj->subBox[0][1] = 0.6f;
        obj->subBox[0][2] = 0.7f;
        obj->subBox[0][3] = 1.3f;
        obj->subBox[0][4] = -0.6f;
        obj->subBox[0][5] = 0.6f;

        obj->subBox[1][0] = -0.5f;
        obj->subBox[1][1] = 0.5f;
        obj->subBox[1][2] = 1.3f;
        obj->subBox[1][3] = 2.6f;
        obj->subBox[1][4] = -0.80f;
        obj->subBox[1][5] = 0.05f;

        obj->subBox[2][0] = -0.55f;
        obj->subBox[2][1] = 0.55f;
        obj->subBox[2][2] = 0.0f;
        obj->subBox[2][3] = 0.7f;
        obj->subBox[2][4] = -0.55f;
        obj->subBox[2][5] = 0.55f;
        return;
    }

    if (nameHasPrefix(obj->name, "BarChair"))
    {
        obj->subBoxCount = 3;

        obj->subBox[0][0] = -0.65f;
        obj->subBox[0][1] = 0.65f;
        obj->subBox[0][2] = 0.0f;
        obj->subBox[0][3] = 2.2f;
        obj->subBox[0][4] = -0.55f;
        obj->subBox[0][5] = 0.55f;

        obj->subBox[1][0] = -0.65f;
        obj->subBox[1][1] = 0.65f;
        obj->subBox[1][2] = 2.30f;
        obj->subBox[1][3] = 2.95f;
        obj->subBox[1][4] = -0.65f;
        obj->subBox[1][5] = 0.65f;

        obj->subBox[2][0] = -0.70f;
        obj->subBox[2][1] = 0.70f;
        obj->subBox[2][2] = 2.40f;
        obj->subBox[2][3] = 4.35f;
        obj->subBox[2][4] = -0.65f;
        obj->subBox[2][5] = -0.05f;
        return;
    }

    if (nameHasPrefix(obj->name, "MeetingTable"))
    {
        obj->subBoxCount = 2;

        obj->subBox[0][0] = -3.0f;
        obj->subBox[0][1] = 3.0f;
        obj->subBox[0][2] = 1.55f;
        obj->subBox[0][3] = 1.85f;
        obj->subBox[0][4] = -1.7f;
        obj->subBox[0][5] = 1.7f;

        obj->subBox[1][0] = -2.35f;
        obj->subBox[1][1] = 2.35f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 1.6f;
        obj->subBox[1][4] = -0.72f;
        obj->subBox[1][5] = 0.72f;
        return;
    }

    if (nameHasPrefix(obj->name, "Lamp"))
    {
        obj->subBoxCount = 3;

        obj->subBox[0][0] = -0.75f;
        obj->subBox[0][1] = 0.75f;
        obj->subBox[0][2] = 4.9f;
        obj->subBox[0][3] = 5.7f;
        obj->subBox[0][4] = -0.75f;
        obj->subBox[0][5] = 0.75f;

        obj->subBox[1][0] = -0.25f;
        obj->subBox[1][1] = 0.25f;
        obj->subBox[1][2] = 0.0f;
        obj->subBox[1][3] = 5.15f;
        obj->subBox[1][4] = -0.25f;
        obj->subBox[1][5] = 0.25f;

        obj->subBox[2][0] = -0.45f;
        obj->subBox[2][1] = 0.45f;
        obj->subBox[2][2] = 0.0f;
        obj->subBox[2][3] = 0.2f;
        obj->subBox[2][4] = -0.45f;
        obj->subBox[2][5] = 0.45f;
        return;
    }

    if (nameHasPrefix(obj->name, "Whiteboard"))
    {
        obj->subBoxCount = 1;
        obj->subBox[0][0] = -0.2f;
        obj->subBox[0][1] = 0.2f;
        obj->subBox[0][2] = 2.0f;
        obj->subBox[0][3] = 6.2f;
        obj->subBox[0][4] = -3.2f;
        obj->subBox[0][5] = 3.2f;
        return;
    }

    if (strcmp(obj->name, "Wall_Back") == 0 ||
        strcmp(obj->name, "Wall_Front") == 0 ||
        strcmp(obj->name, "Wall_Left") == 0 ||
        strcmp(obj->name, "Wall_Right") == 0 ||
        strcmp(obj->name, "Stage") == 0)
    {
        return;
    }

    obj->subBoxCount = 1;
    obj->subBox[0][0] = -0.8f;
    obj->subBox[0][1] = 0.8f;
    obj->subBox[0][2] = 0.0f;
    obj->subBox[0][3] = 3.0f;
    obj->subBox[0][4] = -0.8f;
    obj->subBox[0][5] = 0.8f;
}

static int findFreeGroundSpot(SceneObject *prototype, float *outX, float *outZ)
{
    if (!prototype || prototype->subBoxCount == 0)
        return 0;

    const float margin = 0.5f;
    const float step = 1.0f;
    float minX = ROOM_MIN_X + margin;
    float maxX = ROOM_MAX_X - margin;
    float minZ = ROOM_MIN_Z + margin;
    float maxZ = ROOM_MAX_Z - margin;
    float bestDist = 1e9f;
    int found = 0;

    for (float z = minZ; z <= maxZ; z += step)
    {
        for (float x = minX; x <= maxX; x += step)
        {
            if (!collidesWithAnyObject(prototype, x, z))
            {
                float dist = x * x + z * z;
                if (!found || dist < bestDist)
                {
                    bestDist = dist;
                    *outX = x;
                    *outZ = z;
                    found = 1;
                }
            }
        }
    }

    return found;
}

SceneObject *scene_spawn_object(SceneSpawnType type)
{
    if (type < 0 || type >= SPAWN_TYPE_COUNT)
        return NULL;

    const ObjectTemplate *tmpl = &spawnTemplates[type];

    SceneObject prototype;
    memset(&prototype, 0, sizeof(SceneObject));
    strncpy(prototype.name, tmpl->baseName, sizeof(prototype.name) - 1);
    prototype.name[sizeof(prototype.name) - 1] = '\0';
    prototype.x = 0.0f;
    prototype.y = 0.0f;
    prototype.z = 0.0f;
    prototype.drawFunc = tmpl->drawFunc;
    prototype.movable = tmpl->movable;
    prototype.scale = tmpl->defaultScale;
    prototype.rotation = tmpl->defaultRotation;
    prototype.solid = 1;

    configureObjectBounds(&prototype);

    float spawnX = 0.0f;
    float spawnZ = 0.0f;
    if (!findFreeGroundSpot(&prototype, &spawnX, &spawnZ))
    {
        printf("No available space for %s.\n", tmpl->baseName);
        return NULL;
    }

    char uniqueName[32];
    int count = ++spawnCounters[type];
    snprintf(uniqueName, sizeof(uniqueName), "%s_New%d", tmpl->baseName, count);

    SceneObject *obj = addObject(uniqueName, spawnX, spawnZ, tmpl->drawFunc, tmpl->movable);
    if (!obj)
    {
        printf("Maximum object count reached.\n");
        return NULL;
    }

    obj->rotation = tmpl->defaultRotation;
    obj->scale = tmpl->defaultScale;
    configureObjectBounds(obj);

    selectedObject = obj;
    dragging = 0;

    printf("Spawned %s at (%.1f, %.1f).\n", obj->name, spawnX, spawnZ);
    return obj;
}

void scene_remove_selected_object(void)
{
    if (!selectedObject)
    {
        printf("No object selected to remove.\n");
        return;
    }

    int index = -1;
    for (int i = 0; i < objectCount; i++)
    {
        if (&objects[i] == selectedObject)
        {
            index = i;
            break;
        }
    }

    if (index < 0)
        return;

    if (!objects[index].movable)
    {
        printf("Object %s cannot be removed.\n", objects[index].name);
        return;
    }

    printf("Removed %s.\n", objects[index].name);

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

    if (mode != 2)
    {
        // Ceiling
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 15, -30, 20, 15, 30, 0, -1, 0, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    if (mode != 2)
    {
        // Back wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, -30, 20, 15, -30, 0, 0, 1, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    if (mode != 2)
    {
        // Front wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, 30, 20, 15, 30, 0, 0, -1, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

    if (mode != 2)
    {
        // Left wall
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTex);
        drawTiledSurface(-20, 0, -30, -20, 15, 30, 1, 0, 0, 2.0);
        glDisable(GL_TEXTURE_2D);
    }

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
        SceneObject *obj = &objects[i];

        glPushMatrix();
        glTranslatef(obj->x, obj->y, obj->z);
        glRotatef(obj->rotation, 0, 1, 0);
        glScalef(obj->scale, obj->scale, obj->scale);
        glColor3f(1.0f, 1.0f, 1.0f);

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

        // Highlight selected object
        if (selectedObject == obj)
        {
            glPushMatrix();
            glTranslatef(obj->x, obj->y, obj->z);
            glRotatef(obj->rotation, 0, 1, 0);
            glScalef(obj->scale, obj->scale, obj->scale);

            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);

            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1, -1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glLineWidth(3.0f);
            glColor3f(1.0f, 0.0f, 0.0f);

            if (obj->drawFunc)
                obj->drawFunc(0, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);

            glPopMatrix();
        }
    }
    if (mode != 2)
    {
        // Ceiling lights
        glPushMatrix();
        drawCeilingLights();
        glPopMatrix();
    }

    lighting_draw_debug_marker();
    glPopMatrix();
}

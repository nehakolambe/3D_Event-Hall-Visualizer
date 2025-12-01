#include "CSCIx229.h"

#define MAX_STROKES 8192

int whiteboardMode = 0;

static int previousMode = 0;
static double prevFpvX = 0.0;
static double prevFpvY = 0.0;
static double prevFpvZ = 0.0;
static double prevYaw = 0.0;
static double prevPitch = 0.0;
static double prevAzimuthDeg = 0.0;
static double prevElevationDeg = 0.0;
static double prevViewRadius = 0.0;

static Stroke strokes[MAX_STROKES];
static int strokeCount = 0;

#define WHITEBOARD_SIZE_RATIO 0.65f

static unsigned char *backgroundPixels = NULL;
static GLuint backgroundTexture = 0;
static int backgroundWidth = 0;
static int backgroundHeight = 0;
static int backgroundValid = 0;

static void computeCanvasRect(float *outX, float *outY, float *outW, float *outH)
{
    float width = screenWidth * WHITEBOARD_SIZE_RATIO;
    float height = screenHeight * WHITEBOARD_SIZE_RATIO;

    *outX = (screenWidth - width) * 0.5f;
    *outY = (screenHeight - height) * 0.5f;
    *outW = width;
    *outH = height;
}

void whiteboard_clear(void)
{
    strokeCount = 0;
}

void whiteboard_background_invalidate(void)
{
    backgroundValid = 0;
}

int whiteboard_background_ready(void)
{
    return backgroundValid && backgroundTexture != 0;
}

void whiteboard_capture_background(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    size_t needed = (size_t)width * height * 3;
    if (!backgroundPixels || width != backgroundWidth || height != backgroundHeight)
    {
        free(backgroundPixels);
        backgroundPixels = (unsigned char *)malloc(needed);
        if (!backgroundPixels)
        {
            backgroundWidth = backgroundHeight = 0;
            backgroundValid = 0;
            return;
        }
        backgroundWidth = width;
        backgroundHeight = height;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, backgroundPixels);

    if (!backgroundTexture)
        glGenTextures(1, &backgroundTexture);

    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, backgroundPixels);

    backgroundValid = 1;
}

void whiteboard_draw_background(int width, int height)
{
    if (!whiteboard_background_ready())
        return;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f((float)width, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f((float)width, (float)height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, (float)height);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

void whiteboard_get_canvas_rect(float *x, float *y, float *width, float *height)
{
    computeCanvasRect(x, y, width, height);
}

int whiteboard_point_in_canvas(float sx, float sy)
{
    float x, y, w, h;
    computeCanvasRect(&x, &y, &w, &h);
    return (sx >= x && sx <= x + w && sy >= y && sy <= y + h);
}

void whiteboard_screen_to_canvas(float sx, float sy, float *u, float *v)
{
    float x, y, w, h;
    computeCanvasRect(&x, &y, &w, &h);

    if (w <= 0.0f || h <= 0.0f)
    {
        *u = 0.0f;
        *v = 0.0f;
        return;
    }

    *u = (sx - x) / w;
    *v = (sy - y) / h;

    if (*u < 0.0f)
        *u = 0.0f;
    if (*u > 1.0f)
        *u = 1.0f;
    if (*v < 0.0f)
        *v = 0.0f;
    if (*v > 1.0f)
        *v = 1.0f;
}

void whiteboard_add_stroke(float u1, float v1, float u2, float v2, int erase)
{
    if (strokeCount >= MAX_STROKES)
    {
        memmove(strokes, strokes + 1, sizeof(Stroke) * (MAX_STROKES - 1));
        strokeCount = MAX_STROKES - 1;
    }

    if (u1 < 0.0f)
        u1 = 0.0f;
    if (u1 > 1.0f)
        u1 = 1.0f;
    if (u2 < 0.0f)
        u2 = 0.0f;
    if (u2 > 1.0f)
        u2 = 1.0f;
    if (v1 < 0.0f)
        v1 = 0.0f;
    if (v1 > 1.0f)
        v1 = 1.0f;
    if (v2 < 0.0f)
        v2 = 0.0f;
    if (v2 > 1.0f)
        v2 = 1.0f;

    strokes[strokeCount].u1 = u1;
    strokes[strokeCount].v1 = v1;
    strokes[strokeCount].u2 = u2;
    strokes[strokeCount].v2 = v2;
    strokes[strokeCount].erase = erase;
    strokeCount++;
}

void whiteboard_activate(void)
{
    if (whiteboardMode)
        return;

    previousMode = mode;
    prevFpvX = fpvX;
    prevFpvY = fpvY;
    prevFpvZ = fpvZ;
    prevYaw = yaw;
    prevPitch = pitch;
    prevAzimuthDeg = th;
    prevElevationDeg = ph;
    prevViewRadius = dim;

    whiteboardMode = 1;
    dragging = 0;
    selectedObject = NULL;
    whiteboard_background_invalidate();
}

void whiteboard_deactivate(void)
{
    if (!whiteboardMode)
        return;

    whiteboardMode = 0;
    mode = previousMode;
    fpvX = prevFpvX;
    fpvY = prevFpvY;
    fpvZ = prevFpvZ;
    yaw = prevYaw;
    pitch = prevPitch;
    th = prevAzimuthDeg;
    ph = prevElevationDeg;
    dim = prevViewRadius;
    whiteboard_background_invalidate();
}

void whiteboard_render(float x, float y, float width, float height)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    float shadowOffset = 12.0f;
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(x + shadowOffset, y - shadowOffset);
    glVertex2f(x + width + shadowOffset, y - shadowOffset);
    glVertex2f(x + width + shadowOffset, y + height - shadowOffset);
    glVertex2f(x + shadowOffset, y + height - shadowOffset);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    glLineWidth(4.0f);
    for (int i = 0; i < strokeCount; i++)
    {
        Stroke *stroke = &strokes[i];
        if (stroke->erase)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);
        glVertex2f(x + stroke->u1 * width, y + stroke->v1 * height);
        glVertex2f(x + stroke->u2 * width, y + stroke->v2 * height);
        glEnd();
    }

    glLineWidth(1.0f);
}

void whiteboard_render_on_board(float boardBottom, float boardTop, float boardLeft, float boardRight)
{
    if (strokeCount <= 0)
        return;

    float boardWidth = boardRight - boardLeft;
    float boardHeight = boardTop - boardBottom;

    if (boardWidth <= 0.0f || boardHeight <= 0.0f)
        return;

    GLboolean lighting = glIsEnabled(GL_LIGHTING);
    GLboolean texturing = glIsEnabled(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glLineWidth(3.0f);
    for (int i = 0; i < strokeCount; i++)
    {
        Stroke *stroke = &strokes[i];
        float z1 = boardRight - stroke->u1 * boardWidth;
        float z2 = boardRight - stroke->u2 * boardWidth;
        float y1 = boardBottom + stroke->v1 * boardHeight;
        float y2 = boardBottom + stroke->v2 * boardHeight;

        if (stroke->erase)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);
        glVertex3f(0.01f, y1, z1);
        glVertex3f(0.01f, y2, z2);
        glEnd();
    }

    glLineWidth(1.0f);

    if (texturing)
        glEnable(GL_TEXTURE_2D);
    if (lighting)
        glEnable(GL_LIGHTING);
}

void drawWhiteboardOverlay(void)
{
    float canvasX, canvasY, canvasWidth, canvasHeight;
    whiteboard_get_canvas_rect(&canvasX, &canvasY, &canvasWidth, &canvasHeight);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f((float)screenWidth, 0.0f);
    glVertex2f((float)screenWidth, (float)screenHeight);
    glVertex2f(0.0f, (float)screenHeight);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -4.0f);
    glVertex2f((float)screenWidth + 4.0f, -4.0f);
    glVertex2f((float)screenWidth + 4.0f, (float)screenHeight + 4.0f);
    glVertex2f(-4.0f, (float)screenHeight + 4.0f);
    glEnd();

    whiteboard_render(canvasX, canvasY, canvasWidth, canvasHeight);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

// Simple flat whiteboard on the left wall
void drawWhiteboardTrigger(float x, float z)
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

int whiteboard_get_stroke_count(void)
{
    return strokeCount;
}

Stroke whiteboard_get_stroke(int index)
{
    if (index < 0 || index >= strokeCount)
    {
        // Return an empty stroke if index is invalid
        Stroke empty = {0};
        return empty;
    }
    return strokes[index];
}

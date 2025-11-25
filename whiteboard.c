#include "CSCIx229.h"

#define MAX_STROKES 8192

typedef struct
{
    float u1, v1;
    float u2, v2;
    int erase;
} Stroke;

int whiteboardMode = 0;

static int previousMode = 0;
static double prevFpvX = 0.0;
static double prevFpvY = 0.0;
static double prevFpvZ = 0.0;
static double prevYaw = 0.0;
static double prevPitch = 0.0;
static double prevTh = 0.0;
static double prevPh = 0.0;
static double prevDim = 0.0;

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

    if (u1 < 0.0f) u1 = 0.0f;
    if (u1 > 1.0f) u1 = 1.0f;
    if (u2 < 0.0f) u2 = 0.0f;
    if (u2 > 1.0f) u2 = 1.0f;
    if (v1 < 0.0f) v1 = 0.0f;
    if (v1 > 1.0f) v1 = 1.0f;
    if (v2 < 0.0f) v2 = 0.0f;
    if (v2 > 1.0f) v2 = 1.0f;

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
    prevTh = th;
    prevPh = ph;
    prevDim = dim;

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
    th = prevTh;
    ph = prevPh;
    dim = prevDim;
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
        Stroke *s = &strokes[i];
        if (s->erase)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);
        glVertex2f(x + s->u1 * width, y + s->v1 * height);
        glVertex2f(x + s->u2 * width, y + s->v2 * height);
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
        Stroke *s = &strokes[i];
        float z1 = boardRight - s->u1 * boardWidth;
        float z2 = boardRight - s->u2 * boardWidth;
        float y1 = boardBottom + s->v1 * boardHeight;
        float y2 = boardBottom + s->v2 * boardHeight;

        if (s->erase)
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

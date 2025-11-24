#include "CSCIx229.h"

static int lastMouseX = 0;
static int lastMouseY = 0;
static int strokeActive = 0;
static int strokeErase = 0;
static float strokePrevU = 0.0f;
static float strokePrevV = 0.0f;
static double lastRightClickTime = 0.0;

// room boundaries
static const float ROOM_MIN_X = -18.0f;
static const float ROOM_MAX_X = 18.0f;
static const float ROOM_MIN_Z = -28.0f;
static const float ROOM_MAX_Z = 28.0f;

// stage boundaries
static const float STAGE_MIN_X = -13.0f;
static const float STAGE_MAX_X = 13.0f;
static const float STAGE_FRONT_Z = -15.0f;
static const float STAGE_BACK_Z = -30.0f;

// Ray vs AABB sub-box test
static int rayIntersectsSubBoxWorld(
    float ox, float oy, float oz,
    float dx, float dy, float dz,
    const SceneObject *obj,
    int boxIndex,
    float *tmin_out)
{
    float tmin = -1e9f;
    float tmax = +1e9f;

    for (int axis = 0; axis < 3; axis++)
    {
        float minB = obj->subBox[boxIndex][axis * 2] +
                     (axis == 0 ? obj->x : axis == 1 ? obj->y
                                                     : obj->z);

        float maxB = obj->subBox[boxIndex][axis * 2 + 1] +
                     (axis == 0 ? obj->x : axis == 1 ? obj->y
                                                     : obj->z);

        float o = (axis == 0 ? ox : axis == 1 ? oy
                                              : oz);
        float d = (axis == 0 ? dx : axis == 1 ? dy
                                              : dz);

        if (fabsf(d) < 1e-6f)
        {
            if (o < minB || o > maxB)
                return 0;
        }
        else
        {
            float t1 = (minB - o) / d;
            float t2 = (maxB - o) / d;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
            }

            if (t1 > tmin)
                tmin = t1;
            if (t2 < tmax)
                tmax = t2;

            if (tmin > tmax || tmax < 0)
                return 0;
        }
    }

    *tmin_out = tmin;
    return 1;
}

// Generate world space ray from mouse position
static void getRayFromMouse(int x, int y,
                            float *ox, float *oy, float *oz,
                            float *dx, float *dy, float *dz)
{
    GLdouble model[16], proj[16];
    GLint view[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);

    GLdouble wx1, wy1, wz1;
    GLdouble wx2, wy2, wz2;

    gluUnProject((GLdouble)x, (GLdouble)(view[3] - y), 0.0,
                 model, proj, view, &wx1, &wy1, &wz1);
    gluUnProject((GLdouble)x, (GLdouble)(view[3] - y), 1.0,
                 model, proj, view, &wx2, &wy2, &wz2);

    *ox = (float)wx1;
    *oy = (float)wy1;
    *oz = (float)wz1;

    *dx = (float)(wx2 - wx1);
    *dy = (float)(wy2 - wy1);
    *dz = (float)(wz2 - wz1);
}

// Object picking function
SceneObject *pickObject3D(int x, int y)
{
    float ox, oy, oz, dx, dy, dz;
    getRayFromMouse(x, y, &ox, &oy, &oz, &dx, &dy, &dz);

    SceneObject *closest = NULL;
    float closestT = 1e9f;

    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *obj = &objects[i];
        if (!obj->movable && strcmp(obj->name, "Whiteboard") != 0)
            continue;

        for (int b = 0; b < obj->subBoxCount; b++)
        {
            float tmin;
            if (rayIntersectsSubBoxWorld(ox, oy, oz, dx, dy, dz, obj, b, &tmin))
            {
                if (tmin < closestT)
                {
                    closestT = tmin;
                    closest = obj;
                }
            }
        }
    }

    return closest;
}

// Mouse button callback
void mouse_button(int button, int state, int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;

    if (whiteboardMode)
    {
        if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
        {
            if (state == GLUT_DOWN)
            {
                if (button == GLUT_RIGHT_BUTTON)
                {
                    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
                    if (currentTime - lastRightClickTime < 0.3)
                    {
                        whiteboard_clear();
                        whiteboard_background_invalidate();
                        strokeActive = 0;
                        glutPostRedisplay();
                        lastRightClickTime = 0.0;
                        return;
                    }
                    lastRightClickTime = currentTime;
                }

                float screenX = (float)x;
                float screenY = (float)(screenHeight - y);
                if (whiteboard_point_in_canvas(screenX, screenY))
                {
                    strokeActive = 1;
                    strokeErase = (button == GLUT_RIGHT_BUTTON);
                    whiteboard_screen_to_canvas(screenX, screenY, &strokePrevU, &strokePrevV);
                }
                else
                {
                    strokeActive = 0;
                }
            }
            else if (state == GLUT_UP)
            {
                strokeActive = 0;
            }
        }
        glutPostRedisplay();
        return;
    }

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            SceneObject *picked = pickObject3D(x, y);

            if (picked && strcmp(picked->name, "Whiteboard") == 0)
            {
                whiteboard_activate();
                strokeActive = 0;
                glutPostRedisplay();
                return;
            }

            if (picked)
            {
                if (selectedObject == picked)
                {
                    dragging = 1;
                }
                else
                {
                    selectedObject = picked;
                    dragging = 1;
                    printf("Selected object: %s\n", selectedObject->name);
                }
            }
            else
            {
                selectedObject = NULL;
                dragging = 0;
                printf("Deselected all objects.\n");
            }
        }
        else if (state == GLUT_UP)
        {
            dragging = 0;
            printf("Object placed.\n");
        }
    }

    glutPostRedisplay();
}

// Ray-plane intersection helper
static int rayPlaneIntersection(float ox, float oy, float oz,
                                float dx, float dy, float dz,
                                float *hitX, float *hitZ)
{
    if (fabsf(dy) < 1e-6f)
        return 0;

    float t = -oy / dy;
    if (t < 0)
        return 0;

    *hitX = ox + t * dx;
    *hitZ = oz + t * dz;
    return 1;
}

// Mouse motion callback
void mouse_motion(int x, int y)
{
    if (!whiteboardMode)
    {
        strokeActive = 0;
    }

    if (whiteboardMode)
    {
        if (strokeActive)
        {
            float screenX = (float)x;
            float screenY = (float)(screenHeight - y);

            if (!whiteboard_point_in_canvas(screenX, screenY))
            {
                strokeActive = 0;
            }
            else
            {
                float u, v;
                whiteboard_screen_to_canvas(screenX, screenY, &u, &v);
                whiteboard_add_stroke(strokePrevU, strokePrevV, u, v, strokeErase);
                strokePrevU = u;
                strokePrevV = v;
            }
        }

        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
        return;
    }

    if (dragging && selectedObject)
    {
        float ox, oy, oz, dx, dy, dz;
        getRayFromMouse(x, y, &ox, &oy, &oz, &dx, &dy, &dz);

        float hitX, hitZ;
        if (rayPlaneIntersection(ox, oy, oz, dx, dy, dz, &hitX, &hitZ))
        {
            // room boundaries
            if (hitX < ROOM_MIN_X)
                hitX = ROOM_MIN_X;
            if (hitX > ROOM_MAX_X)
                hitX = ROOM_MAX_X;
            if (hitZ < ROOM_MIN_Z)
                hitZ = ROOM_MIN_Z;
            if (hitZ > ROOM_MAX_Z)
                hitZ = ROOM_MAX_Z;

            // object–object collision
            if (!collidesWithAnyObject(selectedObject, hitX, hitZ))
            {
                selectedObject->x = hitX;
                selectedObject->z = hitZ;
            }
        }
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

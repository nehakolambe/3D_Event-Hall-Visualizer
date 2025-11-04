#include "CSCIx229.h"
#include <math.h>

// =======================================================
//                  INTERNAL STATE
// =======================================================
static int lastMouseX = 0;
static int lastMouseY = 0;

// Compute intersection of the mouse ray with y = 0 plane
static int rayPlaneIntersection(float ox, float oy, float oz,
                                float dx, float dy, float dz,
                                float* hitX, float* hitZ)
{
    if (fabs(dy) < 1e-6) return 0; // parallel to floor
    float t = -oy / dy;
    if (t < 0) return 0;
    *hitX = ox + t * dx;
    *hitZ = oz + t * dz;
    return 1;
}

// =======================================================
//             RAY–AABB INTERSECTION CHECK
// =======================================================
static int rayIntersectsBox(float ox, float oy, float oz,
                            float dx, float dy, float dz,
                            const float bbox[6],
                            float* tmin_out)
{
    float tmin = -1e9f, tmax = 1e9f;
    for (int i = 0; i < 3; i++)
    {
        float minB = bbox[i * 2];
        float maxB = bbox[i * 2 + 1];
        float o = (i == 0 ? ox : (i == 1 ? oy : oz));
        float d = (i == 0 ? dx : (i == 1 ? dy : dz));

        if (fabs(d) < 1e-6)
        {
            if (o < minB || o > maxB) return 0;
        }
        else
        {
            float t1 = (minB - o) / d;
            float t2 = (maxB - o) / d;
            if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax || tmax < 0) return 0;
        }
    }
    if (tmin_out) *tmin_out = tmin;
    return 1;
}

// =======================================================
//           GENERATE A 3D RAY FROM SCREEN (x,y)
// =======================================================
static void getRayFromMouse(int x, int y, float* ox, float* oy, float* oz,
                            float* dx, float* dy, float* dz)
{
    GLdouble model[16], proj[16];
    GLint view[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);

    GLdouble wx1, wy1, wz1, wx2, wy2, wz2;
    gluUnProject((GLdouble)x, (GLdouble)(view[3] - y), 0.0, model, proj, view, &wx1, &wy1, &wz1);
    gluUnProject((GLdouble)x, (GLdouble)(view[3] - y), 1.0, model, proj, view, &wx2, &wy2, &wz2);

    *ox = (float)wx1;
    *oy = (float)wy1;
    *oz = (float)wz1;

    *dx = (float)(wx2 - wx1);
    *dy = (float)(wy2 - wy1);
    *dz = (float)(wz2 - wz1);
}

// =======================================================
//                 OBJECT PICKING
// =======================================================
SceneObject* pickObject3D(int x, int y)
{
    float ox, oy, oz, dx, dy, dz;
    getRayFromMouse(x, y, &ox, &oy, &oz, &dx, &dy, &dz);

    SceneObject* closest = NULL;
    float closestT = 1e9f;

    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* obj = &objects[i];
        if (!obj->movable) continue;

        // Translate ray into object’s local coordinates
        float lx = ox - obj->x;
        float ly = oy - obj->y;
        float lz = oz - obj->z;

        float t;
        if (rayIntersectsBox(lx, ly, lz, dx, dy, dz, obj->bbox, &t))
        {
            if (t < closestT)
            {
                closestT = t;
                closest = obj;
            }
        }
    }

    return closest;
}

// =======================================================
//          HANDLE MOUSE BUTTON EVENTS
// =======================================================
void mouse_button(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            lastMouseX = x;
            lastMouseY = y;

            if (!dragging)
            {
                selectedObject = pickObject3D(x, y);
                if (selectedObject)
                {
                    dragging = 1;
                    printf("Selected object: %s\n", selectedObject->name);
                }
            }
            else
            {
                dragging = 0;
                selectedObject = NULL;
                printf("Object released.\n");
            }
        }
    }

    glutPostRedisplay();
}

// =======================================================
//          HANDLE MOUSE MOVEMENT (DRAGGING)
// =======================================================
void mouse_motion(int x, int y)
{
    if (dragging && selectedObject)
    {
        float ox, oy, oz, dx, dy, dz;
        getRayFromMouse(x, y, &ox, &oy, &oz, &dx, &dy, &dz);

        float hitX, hitZ;
        if (rayPlaneIntersection(ox, oy, oz, dx, dy, dz, &hitX, &hitZ))
        {
            selectedObject->x = hitX;
            selectedObject->z = hitZ;
        }
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}


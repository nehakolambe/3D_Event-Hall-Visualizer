#include "CSCIx229.h"
#include <math.h>
#include <stdbool.h>

extern bool collidesWithAnyObject(SceneObject* movingObj, float newX, float newZ);

static int lastMouseX = 0;
static int lastMouseY = 0;

// =======================================================
//        ROOM BOUNDARIES (MATCHES SCENE.C DIMENSIONS)
// =======================================================
static const float ROOM_MIN_X = -18.0f;
static const float ROOM_MAX_X = 18.0f;
static const float ROOM_MIN_Z = -28.0f;
static const float ROOM_MAX_Z = 28.0f;

// Stage boundaries (from scene_display)
static const float STAGE_MIN_X = -13.0f;
static const float STAGE_MAX_X =  13.0f;
static const float STAGE_FRONT_Z = -15.0f;  // front edge
static const float STAGE_BACK_Z  = -30.0f;  // back wall

// =======================================================
//           RAY–AABB INTERSECTION CHECK (WORLD SPACE)
// =======================================================
static int rayIntersectsBoxWorld(float ox, float oy, float oz,
                                 float dx, float dy, float dz,
                                 const SceneObject* obj,
                                 float* tmin_out)
{
    float tmin = -1e9f, tmax = 1e9f;

    for (int i = 0; i < 3; i++)
    {
        float minB = obj->bbox[i * 2] + (i == 0 ? obj->x : (i == 1 ? obj->y : obj->z));
        float maxB = obj->bbox[i * 2 + 1] + (i == 0 ? obj->x : (i == 1 ? obj->y : obj->z));
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
//                 OBJECT PICKING (WORLD)
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

        float t;
        if (rayIntersectsBoxWorld(ox, oy, oz, dx, dy, dz, obj, &t))
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
static int rayPlaneIntersection(float ox, float oy, float oz,
                                float dx, float dy, float dz,
                                float* hitX, float* hitZ)
{
    if (fabs(dy) < 1e-6) return 0;
    float t = -oy / dy;
    if (t < 0) return 0;
    *hitX = ox + t * dx;
    *hitZ = oz + t * dz;
    return 1;
}

void mouse_motion(int x, int y)
{
    if (dragging && selectedObject)
    {
        float ox, oy, oz, dx, dy, dz;
        getRayFromMouse(x, y, &ox, &oy, &oz, &dx, &dy, &dz);

        float hitX, hitZ;
        if (rayPlaneIntersection(ox, oy, oz, dx, dy, dz, &hitX, &hitZ))
        {
            // ===== Clamp to room boundaries =====
            if (hitX < ROOM_MIN_X) hitX = ROOM_MIN_X;
            if (hitX > ROOM_MAX_X) hitX = ROOM_MAX_X;
            if (hitZ < ROOM_MIN_Z) hitZ = ROOM_MIN_Z;
            if (hitZ > ROOM_MAX_Z) hitZ = ROOM_MAX_Z;

            // ===== Stage boundary logic =====
            // Prevent entry from front
            if (hitZ < STAGE_FRONT_Z && hitX > STAGE_MIN_X && hitX < STAGE_MAX_X)
                hitZ = STAGE_FRONT_Z;

            // Prevent entry from sides while allowing lateral movement
            if (hitZ < STAGE_FRONT_Z + 1.0f && hitZ > STAGE_BACK_Z - 1.0f)
            {
                if (hitX > STAGE_MIN_X - 0.5f && hitX < STAGE_MIN_X + 0.5f)
                    hitX = STAGE_MIN_X - 0.5f;
                if (hitX > STAGE_MAX_X - 0.5f && hitX < STAGE_MAX_X + 0.5f)
                    hitX = STAGE_MAX_X + 0.5f;
            }

            // ===== Object–object collision logic =====
            if (!collidesWithAnyObject(selectedObject, hitX, hitZ))
            {
                selectedObject->x = hitX;
                selectedObject->z = hitZ;
            }
            else
            {
                // Optional visual/audio feedback
                // printf("Blocked by another object\n");
            }
        }
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

#include "CSCIx229.h"
#include <math.h>

// --- External variables ---
extern double fov;
extern double th, ph;
extern double camZ;
extern double dim;

extern int mode;           
extern double fpvX, fpvY, fpvZ;
extern double yaw, pitch;

// Object system
extern SceneObject objects[MAX_OBJECTS];
extern int objectCount;
extern SceneObject* selectedObject;

// Debug
extern int debugMode;
extern int debugObjectIndex;

// --- Lighting externs (from lighting.c) ---
extern float zh;
extern float lightSpeed;
extern float radius;
extern int moveLight;
extern int specularEnabled;
extern int emissionEnabled;
extern int localViewer;
extern int shininess;

extern int lightOn;

void rotateObject(SceneObject* obj, float angle);

// =======================================================
//                 KEYBOARD HANDLER
// =======================================================
void controls_key(unsigned char key, int x, int y)
{
    double speed = 0.8;
    double radYaw = yaw * 3.14159 / 180.0;

    switch (key)
    {
    // ====================================================
    //              PROFESSOR LIGHTING CONTROLS
    // ====================================================

    case 'l': case 'L':
        moveLight = !moveLight;
        break;

    case 'b': case 'B':
        lightOn = !lightOn;
        break;

    case '[':
        zh -= 5;
        break;

    case ']':
        zh += 5;
        break;

    case '{':
        radius -= 1;
        if (radius < 2) radius = 2;
        break;

    case '}':
        radius += 1;
        if (radius > 40) radius = 40;
        break;

    case 'c': case 'C':
        specularEnabled = !specularEnabled;
        break;

    case 'e': case 'E':
        emissionEnabled = !emissionEnabled;
        break;

    case 'p': case 'P':
        localViewer = !localViewer;
        break;

    case '>':
        shininess += 1;
        if (shininess > 128) shininess = 128;
        break;

    case '<':
        shininess -= 1;
        if (shininess < 0) shininess = 0;
        break;

    // ====================================================
    //                 CAMERA / FOV CONTROLS
    // ====================================================
    case '-':
    case '_':
        fov += 2.0;
        if (fov > 80.0) fov = 80.0;
        break;

    case '+':
    case '=':
        fov -= 2.0;
        if (fov < 25.0) fov = 25.0;
        break;

    // --- Dim control
    case '(':
        if (mode == 0)
        {
            dim -= 1.0;
            if (dim < 5.0) dim = 5.0;
        }
        break;

    case ')':
        if (mode == 0)
        {
            dim += 1.0;
            if (dim > 60.0) dim = 60.0;
        }
        break;

    // ====================================================
    //                 FPV MOVEMENT
    // ====================================================
    case 'w': case 'W':
        if (mode == 1)
        {
            fpvX += speed * sin(radYaw);
            fpvZ -= speed * cos(radYaw);
        }
        break;

    case 's': case 'S':
        if (mode == 1)
        {
            fpvX -= speed * sin(radYaw);
            fpvZ += speed * cos(radYaw);
        }
        break;

    case 'a': case 'A':
        if (mode == 1)
        {
            fpvX -= speed * cos(radYaw);
            fpvZ -= speed * sin(radYaw);
        }
        break;

    case 'd': case 'D':
        if (mode == 1)
        {
            fpvX += speed * cos(radYaw);
            fpvZ += speed * sin(radYaw);
        }
        break;

    // ====================================================
    //                  MODE TOGGLE
    // ====================================================
    case 'm': case 'M':
        mode = (mode + 1) % 2; 
        break;

    // ====================================================
    //               OBJECT ROTATION
    // ====================================================
    case 'r':
        if (selectedObject)
            rotateObject(selectedObject, 15.0f);
        break;

    case 'R':
        if (selectedObject)
            rotateObject(selectedObject, -15.0f);
        break;

    // ====================================================
    //                      RESET
    // ====================================================
    case '0':
        th = ph = yaw = pitch = 0;
        camZ = 24;
        fpvX = 0;
        fpvZ = 24;
        fov = 55;
        dim = 20;

        moveLight = 1;
        lightOn = 1;
        specularEnabled = 1;
        emissionEnabled = 0;
        localViewer = 0;
        radius = 10.0;
        zh = 0;

        mode = 0;
        debugMode = 0;
        debugObjectIndex = 0;
        selectedObject = NULL;
        break;

    // --- Exit ---
    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

// =======================================================
//                 SPECIAL KEYS
// =======================================================
void controls_special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_F1:
        if (debugObjectIndex == -1)
        {
            debugObjectIndex = 0;
            debugMode = 1;
        }
        else
        {
            debugObjectIndex++;
            if (debugObjectIndex >= objectCount)
            {
                debugObjectIndex = -1;
                debugMode = 0;
            }
        }
        break;
    }

    if (mode == 1)
    {
        // --- FPV look ---
        switch (key)
        {
        case GLUT_KEY_LEFT:  yaw -= 5; break;
        case GLUT_KEY_RIGHT: yaw += 5; break;
        case GLUT_KEY_UP:    pitch += 5; if (pitch > 60) pitch = 60; break;
        case GLUT_KEY_DOWN:  pitch -= 5; if (pitch < -60) pitch = -60; break;
        }
    }
    else
    {
        // --- Orbit --- 
        switch (key)
        {
        case GLUT_KEY_RIGHT: th += 5; break;
        case GLUT_KEY_LEFT:  th -= 5; break;
        case GLUT_KEY_UP:    ph += 5; if (ph > 90)  ph = 90;  break;
        case GLUT_KEY_DOWN:  ph -= 5; if (ph < -90) ph = -90; break;
        }
    }

    th = fmod(th, 360.0);
    glutPostRedisplay();
}

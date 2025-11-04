#include "CSCIx229.h"
#include <math.h>

// --- External variables ---
extern double fov;
extern double th, ph;
extern double camZ;
extern double dim;

extern int mode;           // replaces fpvMode
extern double fpvX, fpvY, fpvZ;
extern double yaw, pitch;


void controls_key(unsigned char key, int x, int y)
{
    double speed = 0.8;
    double radYaw = yaw * 3.14159 / 180.0;

    switch (key)
    {
    // --- FOV Zoom (+ / -) ---
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

    // --- Dim control (() and )) ---
    case '(':
        if (mode == 0)  // only in perspective
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

    // --- FPV movement ---
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

    // --- Mode toggle ---
    case 'm': case 'M':
        mode = (mode + 1) % 2; // 0 ↔ 1 (you can expand this later)
        break;

    // --- Reset ---
    case '0':
        th = ph = yaw = pitch = 0;
        camZ = 24;
        fpvX = 0;
        fpvZ = 24;
        fov = 55;
        dim = 20;
        mode = 0;
        debugMode = 0;
        debugObjectIndex = 0;
        break;

    // --- Exit ---
    case 27:  // ESC
        exit(0);
    }

    glutPostRedisplay();
}

void controls_special(int key, int x, int y)
{
    switch (key)
    {
    // --- Debug controls ---
    case GLUT_KEY_F1:
        if (debugObjectIndex == -1)
        {
            debugObjectIndex = 0;  // start from first object
            debugMode = 1;
        }
        else
        {
            debugObjectIndex++;
            if (debugObjectIndex >= totalObjects)
            {
                debugObjectIndex = -1; // back to full scene
                debugMode = 0;
            }
        }
        break;
    }

    if (mode == 1)
    {
        // --- FPV look direction ---
        switch (key)
        {
        case GLUT_KEY_LEFT:  yaw -= 5; break;
        case GLUT_KEY_RIGHT: yaw += 5; break;
        case GLUT_KEY_UP:    pitch += 5; if (pitch > 60) pitch = 60; break;
        case GLUT_KEY_DOWN:  pitch -= 5; if (pitch < -60) pitch = -60; break;
        }
    }
    else if (mode == 0)
    {
        // --- Perspective camera rotation ---
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

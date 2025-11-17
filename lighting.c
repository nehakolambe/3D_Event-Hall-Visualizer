//
//  lighting.c
//  Modular lighting system for CSCI-style projects
//

#include "CSCIx229.h"

// -------------------------------------------
// GLOBAL LIGHT STATE (professor style)
// -------------------------------------------

// Light rotation angle
float zh = 0.0f;

// Light movement speed
float lightSpeed = 1.0;

// Light position
float lx = 5.0;
float ly = 5.0;
float lz = 0.0;

// Toggles
int moveLight = 1;
int specularEnabled = 1;
int emissionEnabled = 0;
int localViewer = 0;
int lightOn = 1;     // NEW → toggled by B/b

// Material properties
int shininess = 32;
float shinyVector[1] = {32.0};

// Light colors
float ambient[]  = {0.3, 0.3, 0.3, 1.0};
float diffuse[]  = {1.0, 1.0, 1.0, 1.0};
float specular[] = {1.0, 1.0, 1.0, 1.0};

// Emission color
float emission[] = {0.0, 0.0, 0.0, 1.0};

float radius = 10.0;
float lightY = 5.0;


// -----------------------------------------------------
//  lighting_init()
// -----------------------------------------------------
void lighting_init()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}


// -----------------------------------------------------
//  lighting_update()
// -----------------------------------------------------
void lighting_update()
{
    // =============================
    //  LIGHT ON/OFF MASTER SWITCH
    // =============================
    if (!lightOn)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        return;             // skip ALL other lighting work
    }
    else
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }

    // =============================
    //        LIGHT ANIMATION
    // =============================
    if (moveLight)
        zh += lightSpeed;

    lx = radius * Cos(zh);
    lz = radius * Sin(zh);
    ly = lightY;

    float position[] = {lx, ly, lz, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    Print("Light Pos -> x: %.2f y: %.2f z: %.2f angle: %.2f\n", lx, ly, lz, zh);

    // =============================
    //      MATERIAL SETTINGS
    // =============================
    shinyVector[0] = shininess;
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);

    // Specular enable/disable
    if (specularEnabled)
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    else
    {
        float off[] = {0,0,0,1};
        glLightfv(GL_LIGHT0, GL_SPECULAR, off);
    }

    // Emission toggle
    if (emissionEnabled)
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    else
    {
        float off[] = {0,0,0,1};
        glMaterialfv(GL_FRONT, GL_EMISSION, off);
    }

    // Viewer mode
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);
}


// -----------------------------------------------------
//  lighting_draw_debug_marker()
// -----------------------------------------------------
void lighting_draw_debug_marker()
{
    if (!lightOn)
        return;     // No marker if light is off

    glDisable(GL_LIGHTING);

    glColor3f(1,1,1);
    glPushMatrix();
        glTranslatef(lx, ly, lz);
        glutSolidSphere(0.2, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

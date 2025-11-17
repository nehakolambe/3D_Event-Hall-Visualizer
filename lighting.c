//
//  lighting.c
//  Updated with 3-state lighting system:
//  0 = all off
//  1 = moving light
//  2 = lamp bulb
//

#include "CSCIx229.h"

// -------------------------------------------
// GLOBAL LIGHT STATE
// -------------------------------------------
int lightState = 1;   // 1 = moving light default

// Moving light animation
float zh = 0.0f;
float lightSpeed = 1.0;
float radius = 10.0;
float lightY = 5.0;

float lx = 5.0;
float ly = 5.0;
float lz = 0.0;

// Toggles
int moveLight = 1;
int specularEnabled = 1;
int emissionEnabled = 0;
int localViewer = 0;

// Materials
int shininess = 32;
float shinyVector[1] = {32.0};

// Colors
float ambient[]  = {0.3, 0.3, 0.3, 1.0};
float diffuse[]  = {1.0, 1.0, 1.0, 1.0};
float specular[] = {1.0, 1.0, 1.0, 1.0};

// Emission color
float emission[] = {0.0, 0.0, 0.0, 1.0};

// -------------------------------------------
// Lamp bulb position (set your coordinates)
// -------------------------------------------
float lampX = -5.0f;
float lampY = 5.45f;
float lampZ = -5.0f;


// -----------------------------------------------------
//  lighting_init()
// -----------------------------------------------------
void lighting_init()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHT0);     // moving light
    glEnable(GL_LIGHT1);     // lamp light (off by default)
}


// -----------------------------------------------------
//  lighting_update()
// -----------------------------------------------------
void lighting_update()
{
    // ------------------------------
    // MODE 0 → ALL LIGHTS OFF
    // ------------------------------
    if (lightState == 0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        return;
    }

    glEnable(GL_LIGHTING);

    // ------------------------------
    // MODE 1 → MOVING LIGHT ONLY
    // ------------------------------
    if (lightState == 1)
    {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        if (moveLight)
            zh += lightSpeed;

        lx = radius * Cos(zh);
        lz = radius * Sin(zh);
        ly = lightY;

        float pos0[] = {lx, ly, lz, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, pos0);

        glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

        Print("Moving Light -> x: %.2f y: %.2f z: %.2f angle: %.2f\n",
              lx, ly, lz, zh);
    }
    else
        glDisable(GL_LIGHT0);


    // ------------------------------
    // MODE 2 → LAMP BULB ONLY
    // ------------------------------
    if (lightState == 2)
    {
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);

        float bulbPos[] = {lampX, lampY, lampZ, 1.0};
        float bulbAmb[]  = {0.30, 0.25, 0.15, 1.0};   // warm soft ambient
        float bulbDif[]  = {1.00, 0.85, 0.40, 1.0};   // strong warm yellow
        float bulbSpec[] = {0.60, 0.50, 0.30, 1.0};   // warm highlights

        glLightfv(GL_LIGHT1, GL_POSITION, bulbPos);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  bulbAmb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  bulbDif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bulbSpec);

        Print("Lamp Bulb Light -> x: %.2f y: %.2f z: %.2f\n",
              lampX, lampY, lampZ);
    }
    else
        glDisable(GL_LIGHT1);


    // ------------------------------
    // MATERIAL SETTINGS
    // ------------------------------
    shinyVector[0] = shininess;
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);

    // Specular toggle
    if (specularEnabled)
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    else
    {
        float off[] = {0,0,0,1};
        glMaterialfv(GL_FRONT, GL_SPECULAR, off);
    }

    // Emission toggle
    if (emissionEnabled)
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    else
    {
        float off[] = {0,0,0,1};
        glMaterialfv(GL_FRONT, GL_EMISSION, off);
    }

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);
}


// -----------------------------------------------------
//  lighting_draw_debug_marker()
// -----------------------------------------------------
void lighting_draw_debug_marker()
{
    if (lightState != 1) return;   // ONLY show marker for moving light

    glDisable(GL_LIGHTING);

    glColor3f(1,1,1);
    glPushMatrix();
        glTranslatef(lx, ly, lz);
        glutSolidSphere(0.2, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}


// -----------------------------------------------------
//  drawLampBulb()  -- CALL THIS FROM YOUR SCENE
// -----------------------------------------------------
void drawLampBulb()
{
    if (lightState != 2) return;

    glPushMatrix();
    glTranslatef(lampX, lampY, lampZ);

    // Warm golden glow
    float glow[] = {1.0, 0.75, 0.25, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION, glow);

    glutSolidSphere(0.35, 20, 20);

    // Stop glowing for rest of scene
    float off[] = {0,0,0,1};
    glMaterialfv(GL_FRONT, GL_EMISSION, off);

    glPopMatrix();
}


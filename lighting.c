#include "CSCIx229.h"

/* Global light state */
int lightState = 1;

/* Moving light animation */
float zh = 0.0f;
float lightSpeed = 1.0f;
float radius = 10.0f;
float lightY = 5.0f;

float lx = 5.0f;
float ly = 5.0f;
float lz = 0.0f;

/* Toggles */
int moveLight = 1;

/* Materials */
int shininess = 32;
float shinyVector[1] = {32.0f};

/* Light colors */
float ambient[]  = {0.3f, 0.3f, 0.3f, 1.0f};
float diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

float emission[] = {0.0f, 0.0f, 0.0f, 1.0f};


/* Initialize lighting */
void lighting_init(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHT0); // moving light
    glEnable(GL_LIGHT1); // lamp bulb light
}


/* Update lighting each frame */
void lighting_update(void)
{
    if (lightState == 0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        return;
    }

    glEnable(GL_LIGHTING);

    /* Mode 1: moving light */
    if (lightState == 1)
    {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        if (moveLight)
            zh += lightSpeed;

        lx = radius * Cos(zh);
        lz = radius * Sin(zh);
        ly = lightY;

        float pos0[] = {lx, ly, lz, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, pos0);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }
    else
    {
        glDisable(GL_LIGHT0);
    }

    /* Mode 2: lamp bulb only */
    if (lightState == 2)
    {
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT0);

        // drawLamp() sets GL_LIGHT1 position
        float bulbAmb[]  = {0.20f, 0.25f, 0.35f, 1.0f};
        float bulbDif[]  = {0.80f, 0.90f, 1.00f, 1.0f};
        float bulbSpec[] = {1.0f, 1.0f, 0.95f, 1.0f};

        glLightfv(GL_LIGHT1, GL_AMBIENT,  bulbAmb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  bulbDif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bulbSpec);
    }
    else
    {
        glDisable(GL_LIGHT1);
    }

    /* Material settings */
    shinyVector[0] = shininess;
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
}


/* Debug sphere for moving light */
void lighting_draw_debug_marker(void)
{
    if (lightState != 1)
        return;

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(lx, ly, lz);
    glutSolidSphere(0.2, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

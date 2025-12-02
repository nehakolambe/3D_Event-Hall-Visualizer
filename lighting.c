#include "CSCIx229.h"

// global light state
int lightState = 1;

// moving light parameters
float movingLightAngle = 0.0f;
float movingLightSpeed = 1.0f;
float movingLightRadius = 10.0f;
float movingLightHeight = 5.0f;

float movingLightPosX = 5.0f;
float movingLightPosY = 5.0f;
float movingLightPosZ = 0.0f;

// toggle
int movingLightEnabled = 1;

// material shininess
int shininess = 32;
float shinyVector[1] = {32.0f};

// light properties
float ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

float emission[] = {0.0f, 0.0f, 0.0f, 1.0f};

// initialize lighting
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

// update lighting each frame
void lighting_update(void)
{
    // Disable everything when lights are off
    if (lightState == 0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        return;
    }

    glEnable(GL_LIGHTING);

    // mode 1: moving light
    if (lightState == 1)
    {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // Advance the animation when movement is enabled
        if (movingLightEnabled)
            movingLightAngle += movingLightSpeed;

        movingLightPosX = movingLightRadius * Cos(movingLightAngle);
        movingLightPosZ = movingLightRadius * Sin(movingLightAngle);
        movingLightPosY = movingLightHeight;

        float pos0[] = {movingLightPosX, movingLightPosY, movingLightPosZ, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, pos0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }
    else
    {
        glDisable(GL_LIGHT0);
    }

    // mode 2: lamp light
    if (lightState == 2)
    {
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT0);

        // drawLamp() sets GL_LIGHT1 position
        float bulbAmb[] = {0.20f, 0.25f, 0.35f, 1.0f};
        float bulbDif[] = {0.80f, 0.90f, 1.00f, 1.0f};
        float bulbSpec[] = {1.0f, 1.0f, 0.95f, 1.0f};

        glLightfv(GL_LIGHT1, GL_AMBIENT, bulbAmb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, bulbDif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bulbSpec);
    }
    else
    {
        glDisable(GL_LIGHT1);
    }

    // material properties
    shinyVector[0] = shininess;
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
}

// debug sphere
void lighting_draw_debug_marker(void)
{
    // Only draw the marker for the moving light
    if (lightState != 1)
        return;

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(movingLightPosX, movingLightPosY, movingLightPosZ);
    glutSolidSphere(0.2, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

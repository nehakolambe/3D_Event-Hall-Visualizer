#include "CSCIx229.h"

// Toggle for light (0 = Light off, 1 = Moving Light, 2 = Lamp Light)
int lightState = 1;

// Moving light parameters
float movingLightAngle = 0.0f;
float movingLightSpeed = 1.0f;
float movingLightRadius = 10.0f;
float movingLightHeight = 5.0f;

float movingLightPosX = 5.0f;
float movingLightPosY = 5.0f;
float movingLightPosZ = 0.0f;

// Toggle to pause the moving light
int movingLightEnabled = 1;

// Material shininess
int shininess = 32;
float shinyVector[1] = {32.0f};

// Light properties
float ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
float emission[] = {0.0f, 0.0f, 0.0f, 1.0f};

// Initialize lighting
void lighting_init(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    // Allow object colors to interact with lighting
    glEnable(GL_COLOR_MATERIAL);

    // Light reflects off both sides of faces
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Smooth out the lighting
    glShadeModel(GL_SMOOTH);

    // Enabling two lights
    glEnable(GL_LIGHT0); // Moving light
    glEnable(GL_LIGHT1); // Floor lamp
}

// Update lighting each frame
void lighting_update(void)
{
    // Mode 0: Lights Off
    if (lightState == 0)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        return;
    }

    glEnable(GL_LIGHTING);

    // Mode 1: The Moving Light
    if (lightState == 1)
    {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1); // Turn off the lamp light

        // If not paused, move the angle forward
        if (movingLightEnabled)
            movingLightAngle += movingLightSpeed;

        // Make the light go in an orbit
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
        // If not in mode 1, disable light0
        glDisable(GL_LIGHT0);
    }

    // Mode 2: The Floor Lamp
    if (lightState == 2)
    {
        glEnable(GL_LIGHT1);
        glDisable(GL_LIGHT0); // Turn off the moving light

        float bulbAmb[] = {0.20f, 0.25f, 0.35f, 1.0f};
        float bulbDif[] = {0.80f, 0.90f, 1.00f, 1.0f};
        float bulbSpec[] = {1.0f, 1.0f, 0.95f, 1.0f};

        glLightfv(GL_LIGHT1, GL_AMBIENT, bulbAmb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, bulbDif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bulbSpec);
    }
    else
    {
        // If not in mode 2, disable light1
        glDisable(GL_LIGHT1);
    }

    // Apply the shininess settings to all objects
    shinyVector[0] = shininess;
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
}

// Draws a small white ball so we can see where the light is coming from
void lighting_draw_debug_marker(void)
{
    // Only draw the ball for the moving light (Mode 1)
    if (lightState != 1)
        return;

    // Turn off lighting temporarily so the ball is pure bright white
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(movingLightPosX, movingLightPosY, movingLightPosZ);
    glutSolidSphere(0.2, 16, 16); // Draw the ball
    glPopMatrix();

    // Turn lighting on
    glEnable(GL_LIGHTING);
}

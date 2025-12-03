#include "CSCIx229.h"

// projecion parameters
double asp = 1;
double fov = 55;
double dim = 20;
int screenWidth = 800;
int screenHeight = 600;

// camera parameters
double camX = 0.0, camY = 6.0, camZ = 24.0;
double fpvX = 0.0, fpvY = 3.0, fpvZ = 24.0;
double yaw = 0.0, pitch = 0.0;

// scene parameters
double th = 0;
double ph = 0;

// camera mode
int mode = 0;

// set projection
void Project(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (mode == 2)
    {
        // orthogonal projection
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -50, +50);
    }
    else
    {
        // perspective projection
        gluPerspective(fov, asp, 0.1, dim * 10.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

// display callback
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Project();
    glLoadIdentity();

    if (mode == 0)
    {
        // perspective mode
        double camDist = 24.0 + (dim - 20.0);
        gluLookAt(camX, camY, camDist,
                  0.0, 6.0, -20.0,
                  0.0, 1.0, 0.0);
        glRotated(ph, 1, 0, 0);
        glRotated(th, 0, 1, 0);
    }
    else if (mode == 1)
    {
        // FPV mode
        double dirX = Sin(yaw);
        double dirZ = -Cos(yaw);
        double zoomOffset = (55.0 - fov) * 0.1;
        double viewX = fpvX - dirX * zoomOffset;
        double viewZ = fpvZ - dirZ * zoomOffset;

        gluLookAt(viewX, fpvY, viewZ,
                  fpvX + dirX,
                  fpvY + tan(pitch * PI / 180.0),
                  fpvZ + dirZ,
                  0.0, 1.0, 0.0);
    }
    else if (mode == 2)
    {
        // orthogonal mode
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw scene
    scene_display();

    // HUD overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // disable lightting for text overlay
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    const char *modeStr =
        (mode == 0) ? "Perspective" : (mode == 1) ? "FPV"
                                                  : "Orthogonal";
    glWindowPos2f(10, 40);
    Print("Mode: %s   FOV: %.1f°   DIM: %.1f", modeStr, fov, dim);
    glWindowPos2f(10, 80);
    switch (lightState)
    {
    case 1:
        Print("Light Mode: Moving ");
        Print(" Light Radius:%.1f", movingLightRadius);
        Print(" Light Height (Y): %.1f", movingLightHeight);
        break;

    case 2:
        Print("Light Mode: Spotlight");
        break;

    case 0:
        break;
    }
    glWindowPos2f(10, 60);
    if (selectedObject)
    {
        Print("Selected: %s", selectedObject->name);
        Print(" Angle: %.1f°", selectedObject->rotation);
    }
    else
    {
        Print("Selected: None");
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    ErrCheck("display");
    glutSwapBuffers();
}

// window reshape callback
void reshape(int width, int height)
{
    asp = (height > 0) ? (double)width / height : 1;
    screenWidth = width;
    screenHeight = height > 0 ? height : 1;
    glViewport(0, 0, width, height);
    Project();
}

// main program
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("final project: neha kolambe");

#ifdef USEGLEW
    glewInit();
#endif

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glDisable(GL_CULL_FACE);

    // init scene and lighting
    scene_init();
    initPlayerCollision();
    lighting_init();

    // callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(controls_key);
    glutSpecialFunc(controls_special);
    glutMouseFunc(mouse_button);
    glutMotionFunc(mouse_motion);
    glutIdleFunc(display);

    glutMainLoop();
    return 0;
}

#include "CSCIx229.h"

/* Projection parameters */
double asp = 1;
double fov = 55;
double dim = 20;

/* Camera positions */
double camX = 0.0, camY = 6.0, camZ = 24.0;
double fpvX = 0.0, fpvY = 6.0, fpvZ = 24.0;
double yaw = 0.0, pitch = 0.0;

/* Scene rotation */
double th = 0;
double ph = 0;

/* Camera mode: 0 = perspective, 1 = FPV */
int mode = 0;

/* Set projection */
void Project(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, asp, 0.1, dim * 10.0);
    glMatrixMode(GL_MODELVIEW);
}

/* Display callback */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Project();
    glLoadIdentity();

    if (mode == 0)
    {
        // Perspective mode
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
        double dirX = sin(yaw * PI / 180.0);
        double dirZ = -cos(yaw * PI / 180.0);
        double zoomOffset = (55.0 - fov) * 0.1;
        double viewX = fpvX - dirX * zoomOffset;
        double viewZ = fpvZ - dirZ * zoomOffset;

        gluLookAt(viewX, fpvY, viewZ,
                  fpvX + dirX,
                  fpvY + tan(pitch * PI / 180.0),
                  fpvZ + dirZ,
                  0.0, 1.0, 0.0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw scene
    scene_display();

    // HUD overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    glWindowPos2f(10, 10);

    const char *modeStr =
        (mode == 0) ? "Perspective" : (mode == 1) ? "FPV"
                                                  : "Other";

    Print("Mode: %s   FOV: %.1f°   DIM: %.1f", modeStr, fov, dim);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    ErrCheck("display");
    glutSwapBuffers();
}

/* Window reshape */
void reshape(int width, int height)
{
    asp = (height > 0) ? (double)width / height : 1;
    glViewport(0, 0, width, height);
    Project();
}

/* Main entry */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
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

    // Init scene and lighting
    scene_init();
    lighting_init();

    // Callbacks
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

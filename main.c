#include "CSCIx229.h"

// Projection parameters
double asp = 1;
double fov = 55;
double dim = 20;

// Camera positions
double camX = 0.0, camY = 6.0, camZ = 24.0;
double fpvX = 0.0, fpvY = 6.0, fpvZ = 24.0;
double yaw = 0.0, pitch = 0.0;

// Scene rotation (for perspective mode)
double th = 0;
double ph = 0;

// Camera mode: 0 = Perspective, 1 = FPV (more can be added later)
int mode = 0;

// --- Debug toggle ---
int debugMode = 0;        // 0 = scene, 1 = single-object view
int debugObjectIndex = -1; // -1 = scene, 0..N = which object
int totalObjects = 5;     // (table, cocktail table, chair, lamp, door, screen)

// --- Projection setup ---
void Project(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, asp, 0.1, dim * 10.0);
    glMatrixMode(GL_MODELVIEW);
}

// --- Display ---
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Project();
    glLoadIdentity();

    if (mode == 0)
    {
        // --- Perspective mode ---
        double camDist = 24.0 + (dim - 20.0);
        gluLookAt(camX, camY, camDist,
                  0.0, 6.0, -20.0,
                  0.0, 1.0, 0.0);
        glRotated(ph, 1, 0, 0);
        glRotated(th, 0, 1, 0);
    }
    else if (mode == 1)
    {
        // --- FPV mode ---
        double dirX = sin(yaw * 3.14159 / 180.0);
        double dirZ = -cos(yaw * 3.14159 / 180.0);
        double zoomOffset = (55.0 - fov) * 0.1;
        double viewX = fpvX - dirX * zoomOffset;
        double viewZ = fpvZ - dirZ * zoomOffset;

        gluLookAt(viewX, fpvY, viewZ,
                  fpvX + dirX, fpvY + tan(pitch * 3.14159 / 180.0), fpvZ + dirZ,
                  0.0, 1.0, 0.0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    scene_display();

    // === HUD overlay using Print() ===
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    const char* modeStr = (mode == 0) ? "Perspective" :
                          (mode == 1) ? "FPV" :
                          "Other";

    glWindowPos2f(10, 10);
    Print("Mode: %s   FOV: %.1f°   DIM: %.1f", modeStr, fov, dim);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    ErrCheck("display");
    glutSwapBuffers();
}

// --- Reshape ---
void reshape(int width, int height)
{
    asp = (height > 0) ? (double)width / height : 1;
    glViewport(0, 0, width, height);
    Project();
}

// --- Main ---
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Event Hall Visualizer");

#ifdef USEGLEW
    glewInit();
#endif

    glEnable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(controls_key);
    glutSpecialFunc(controls_special);
    glutMainLoop();
    return 0;
}

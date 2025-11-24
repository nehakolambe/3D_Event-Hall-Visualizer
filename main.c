#include "CSCIx229.h"

/* Projection parameters */
double asp = 1;
double fov = 55;
double dim = 20;
int screenWidth = 800;
int screenHeight = 600;

/* Camera positions */
double camX = 0.0, camY = 6.0, camZ = 24.0;
double fpvX = 0.0, fpvY = 3.0, fpvZ = 24.0;
double yaw = 0.0, pitch = 0.0;

/* Scene rotation */
double th = 0;
double ph = 0;

/* Camera mode: 0 = perspective, 1 = FPV */
int mode = 0;

static void drawWhiteboardOverlay(void)
{
    float boardX, boardY, boardW, boardH;
    whiteboard_get_canvas_rect(&boardX, &boardY, &boardW, &boardH);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f((float)screenWidth, 0.0f);
    glVertex2f((float)screenWidth, (float)screenHeight);
    glVertex2f(0.0f, (float)screenHeight);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -4.0f);
    glVertex2f((float)screenWidth + 4.0f, -4.0f);
    glVertex2f((float)screenWidth + 4.0f, (float)screenHeight + 4.0f);
    glVertex2f(-4.0f, (float)screenHeight + 4.0f);
    glEnd();

    whiteboard_render(boardX, boardY, boardW, boardH);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}
/* Set projection */
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

/* Display callback */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    int backgroundReady = whiteboard_background_ready();

    if (whiteboardMode && backgroundReady)
    {
        whiteboard_draw_background(screenWidth, screenHeight);
    }
    else
    {
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
        else if (mode == 2)
        {
            // orthogonal mode
            glRotatef(ph, 1, 0, 0);
            glRotatef(th, 0, 1, 0);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw scene
        scene_display();

        if (whiteboardMode && !backgroundReady)
        {
            whiteboard_capture_background(screenWidth, screenHeight);
            backgroundReady = whiteboard_background_ready();
        }
    }

    if (whiteboardMode)
    {
        drawWhiteboardOverlay();
    }
    else
    {
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
        glWindowPos2f(10, 20);

        const char *modeStr =
            (mode == 0) ? "Perspective" : (mode == 1) ? "FPV"
                                                      : "Orthogonal";
        glWindowPos2f(10, 40);
        // --- Print Camera Info ---
        Print("Mode: %s   FOV: %.1f°   DIM: %.1f", modeStr, fov, dim);
        glWindowPos2f(10, 80);
        // --- Print Lighting Info ---
        switch (lightState)
        {
        case 1:
            Print("Light Mode: Moving ");
            Print(" Light Radius:%.1f", radius);
            Print(" Light Height (Y): %.1f", lightY);
            break;

        case 2:
            Print("Light Mode: Spotlight");
            break;

        case 0:
            // print nothing
            break;
        }
        glWindowPos2f(10, 60);
        // --- Print Selected Object Info ---
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
    }

    ErrCheck("display");
    glutSwapBuffers();
}

/* Window reshape */
void reshape(int width, int height)
{
    asp = (height > 0) ? (double)width / height : 1;
    screenWidth = width;
    screenHeight = height > 0 ? height : 1;
    whiteboard_background_invalidate();
    glViewport(0, 0, width, height);
    Project();
}

/* Main entry */
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

    // Init scene and lighting
    scene_init();
    initPlayerCollision();
    lighting_init();
    whiteboard_clear();

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

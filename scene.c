#include "CSCIx229.h"
#include <math.h>

// Utility to draw colored quad
static void drawQuad(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float x3, float y3, float z3,
                     float x4, float y4, float z4,
                     float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

void scene_display()
{
    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    if (!debugMode)
    {
        // ==== Floor ====
        drawQuad(-20, 0, -30,  20, 0, -30,  20, 0, 30,  -20, 0, 30,  0.6, 0.6, 0.6);

        // ==== Ceiling ====
        drawQuad(-20, 15, -30,  20, 15, -30,  20, 15, 30,  -20, 15, 30,  0.9, 0.9, 0.9);

        // ==== Walls ====
        drawQuad(-20, 0, -30,  20, 0, -30,  20, 15, -30,  -20, 15, -30,  0.7, 0.7, 0.75);
        drawQuad(-20, 0, 30,  20, 0, 30,  20, 15, 30,  -20, 15, 30,  0.7, 0.7, 0.75);
        drawQuad(-20, 0, -30,  -20, 0, 30,  -20, 15, 30,  -20, 15, -30,  0.8, 0.8, 0.85);
        drawQuad(20, 0, -30,  20, 0, 30,  20, 15, 30,  20, 15, -30,  0.8, 0.8, 0.85);

        // ==== Stage ====
        float stageTop = 2.0;
        float stageBack = -30;
        float stageFront = stageBack + 10;
        float stageWidth = 10.0;

        drawQuad(-stageWidth, stageTop, stageBack,  stageWidth, stageTop, stageBack,
                 stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageFront,
                 0.4, 0.2, 0.1);
        drawQuad(-stageWidth, 0, stageFront,  stageWidth, 0, stageFront,
                 stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageFront,
                 0.35, 0.17, 0.09);
        drawQuad(-stageWidth, 0, stageBack,  stageWidth, 0, stageBack,
                 stageWidth, stageTop, stageBack, -stageWidth, stageTop, stageBack,
                 0.35, 0.17, 0.09);
        drawQuad(-stageWidth, 0, stageBack,  -stageWidth, 0, stageFront,
                 -stageWidth, stageTop, stageFront, -stageWidth, stageTop, stageBack,
                 0.32, 0.16, 0.08);
        drawQuad(stageWidth, 0, stageFront,  stageWidth, 0, stageBack,
                 stageWidth, stageTop, stageBack,  stageWidth, stageTop, stageFront,
                 0.32, 0.16, 0.08);

        // ==== Stair ====
        float stairWidth = 8.0, stairHeight = 1.0, stairDepth = 1.5;
        float stepBottom = 0.0, stepTop = stairHeight;
        float stepBackZ = stageFront, stepFrontZ = stageFront + stairDepth;

        drawQuad(-stairWidth, stepTop, stepBackZ, stairWidth, stepTop, stepBackZ,
                 stairWidth, stepTop, stepFrontZ, -stairWidth, stepTop, stepFrontZ,
                 0.45, 0.25, 0.12);
        drawQuad(-stairWidth, stepBottom, stepFrontZ, stairWidth, stepBottom, stepFrontZ,
                 stairWidth, stepTop, stepFrontZ, -stairWidth, stepTop, stepFrontZ,
                 0.35, 0.18, 0.09);
        drawQuad(-stairWidth, stepBottom, stepBackZ, stairWidth, stepBottom, stepBackZ,
                 stairWidth, stepTop, stepBackZ, -stairWidth, stepTop, stepBackZ,
                 0.35, 0.18, 0.09);
        drawQuad(-stairWidth, stepBottom, stepBackZ, -stairWidth, stepBottom, stepFrontZ,
                 -stairWidth, stepTop, stepFrontZ, -stairWidth, stepTop, stepBackZ,
                 0.32, 0.16, 0.08);
        drawQuad(stairWidth, stepBottom, stepFrontZ, stairWidth, stepBottom, stepBackZ,
                 stairWidth, stepTop, stepBackZ, stairWidth, stepTop, stepFrontZ,
                 0.32, 0.16, 0.08);

        // ==== Objects ====
        drawTable(0, 0);
        drawCocktailTable(6, 0);
        drawBanquetChair(-4, 0);
        drawLamp(-6, 0);
        drawDoor(0.0, 29.9, 3.0, 7.0);
        drawCurvedScreen(0, -30, 35.0, 10.0, 3.5, 25.0, 35.0, 0.5);

        // ==== Windows ====
        glColor3f(0.3, 0.5, 0.9);
        drawQuad(-19.9, 6, -15, -19.9, 6, -5, -19.9, 10, -5, -19.9, 10, -15, 0.3, 0.5, 0.9);
        drawQuad(-19.9, 6, 5, -19.9, 6, 15, -19.9, 10, 15, -19.9, 10, 5, 0.3, 0.5, 0.9);
        drawQuad(19.9, 6, -15, 19.9, 6, -5, 19.9, 10, -5, 19.9, 10, -15, 0.3, 0.5, 0.9);
        drawQuad(19.9, 6, 5, 19.9, 6, 15, 19.9, 10, 15, 19.9, 10, 5, 0.3, 0.5, 0.9);
    }
    else
    {
        // ==== Debug Mode: Single Object at Origin ====
        glPushMatrix();
        glTranslatef(0, 0, 0);
        switch (debugObjectIndex)
        {
        case 0: drawTable(0, 0); break;
        case 1: drawCocktailTable(0, 0); break;
        case 2: drawBanquetChair(0, 0); break;
        case 3: drawLamp(0, 0); break;
        case 4: drawCurvedScreen(0, 0, 35.0, 10.0, 0.0, 25.0, 35.0, 0.0); break;
        }
        glPopMatrix();
    }

    glPopMatrix();
}

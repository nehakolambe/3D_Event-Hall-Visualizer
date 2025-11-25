#include "CSCIx229.h"

// Draw one cylindrical leg standing vertically
static void drawLeg(float x, float z, float height, float radius)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3f(1.0f, 1.0f, 1.0f);

    drawCylinder(radius, height, 32);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Draw a textured rectangular table with rounded corners
void drawTable(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);

    float tabletopY = 2.0f;
    float thickness = 0.15f;
    float legHeight = tabletopY;
    float legSize = 0.25f;
    float length = 4.0f;
    float width = 2.0f;
    float radius = 0.3f;
    int segs = 20;

    float yBottom = tabletopY;
    float yTop = tabletopY + thickness;
    float innerX = length * 0.5f - radius;
    float innerZ = width * 0.5f - radius;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableTex);
    glColor3f(1, 1, 1);

    // top surface
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-innerX, yTop, -innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(innerX, yTop, -innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(innerX, yTop, innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX, yTop, innerZ);
    glEnd();

    // flat edge pads
    glBegin(GL_QUADS);

    // front
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-innerX, yTop, innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(innerX, yTop, innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(innerX, yTop, innerZ + radius);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX, yTop, innerZ + radius);

    // back
    glTexCoord2f(0, 0);
    glVertex3f(-innerX, yTop, -innerZ - radius);
    glTexCoord2f(1, 0);
    glVertex3f(innerX, yTop, -innerZ - radius);
    glTexCoord2f(1, 1);
    glVertex3f(innerX, yTop, -innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX, yTop, -innerZ);

    // left
    glTexCoord2f(0, 0);
    glVertex3f(-innerX - radius, yTop, -innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(-innerX, yTop, -innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(-innerX, yTop, innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX - radius, yTop, innerZ);

    // right
    glTexCoord2f(0, 0);
    glVertex3f(innerX, yTop, -innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(innerX + radius, yTop, -innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(innerX + radius, yTop, innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(innerX, yTop, innerZ);

    glEnd();

    // rounded corners (top)
    for (int c = 0; c < 4; c++)
    {
        float cx = (c == 0 || c == 3) ? innerX : -innerX;
        float cz = (c < 2) ? innerZ : -innerZ;

        float startDeg = (float)(c * 90);
        float endDeg = startDeg + 90.0f;

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 1, 0);

        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(cx, yTop, cz);

        for (int i = 0; i <= segs; i++)
        {
            float a = (startDeg + i * (endDeg - startDeg) / segs) * (M_PI / 180.0f);
            float xPos = cx + radius * cosf(a);
            float zPos = cz + radius * sinf(a);

            float u = 0.5f + 0.5f * cosf(a);
            float v = 0.5f + 0.5f * sinf(a);

            glTexCoord2f(u, v);
            glVertex3f(xPos, yTop, zPos);
        }
        glEnd();
    }

    // curved side walls
    for (int c = 0; c < 4; c++)
    {
        float cx = (c == 0 || c == 3) ? innerX : -innerX;
        float cz = (c < 2) ? innerZ : -innerZ;

        float startDeg = (float)(c * 90);
        float endDeg = startDeg + 90.0f;

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segs; i++)
        {
            float a = (startDeg + i * (endDeg - startDeg) / segs) * (M_PI / 180.0f);

            float xPos = cx + radius * cosf(a);
            float zPos = cz + radius * sinf(a);
            float nx = cosf(a);
            float nz = sinf(a);

            glNormal3f(nx, 0, nz);

            float u = (float)i / segs;

            glTexCoord2f(u, 0);
            glVertex3f(xPos, yBottom, zPos);
            glTexCoord2f(u, 1);
            glVertex3f(xPos, yTop, zPos);
        }
        glEnd();
    }

    // flat side walls
    glBegin(GL_QUADS);

    // front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-innerX, yBottom, innerZ + radius);
    glTexCoord2f(1, 0);
    glVertex3f(innerX, yBottom, innerZ + radius);
    glTexCoord2f(1, 1);
    glVertex3f(innerX, yTop, innerZ + radius);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX, yTop, innerZ + radius);

    // back
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(-innerX, yBottom, -innerZ - radius);
    glTexCoord2f(1, 0);
    glVertex3f(innerX, yBottom, -innerZ - radius);
    glTexCoord2f(1, 1);
    glVertex3f(innerX, yTop, -innerZ - radius);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX, yTop, -innerZ - radius);

    // left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-innerX - radius, yBottom, -innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(-innerX - radius, yBottom, innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(-innerX - radius, yTop, innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(-innerX - radius, yTop, -innerZ);

    // right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(innerX + radius, yBottom, -innerZ);
    glTexCoord2f(1, 0);
    glVertex3f(innerX + radius, yBottom, innerZ);
    glTexCoord2f(1, 1);
    glVertex3f(innerX + radius, yTop, innerZ);
    glTexCoord2f(0, 1);
    glVertex3f(innerX + radius, yTop, -innerZ);

    glEnd();

    // legs
    float pos[4][2] = {
        {innerX, innerZ},
        {-innerX, innerZ},
        {-innerX, -innerZ},
        {innerX, -innerZ}};

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(pos[i][0], legHeight * 0.5f, pos[i][1]);
        drawCuboid(legSize, legHeight, legSize);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Cocktail table (circular top, single leg)
void drawCocktailTable(float x, float z)
{
    float height = 4.0f;
    float topRadius = 1.5f;
    float topThickness = 0.1f;
    float legRadius = 0.15f;

    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    drawLeg(0.0f, 0.0f, height, legRadius);    // table leg
    drawDisk(topRadius, height, topThickness); // tabletop
    drawDisk(0.6f, 0.0f, 0.05f);               // base disk

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Draw door with frame and knob
void drawDoor(float x, float z, float width, float height)
{
    float yBottom = 0.0f;
    float yTop = height;
    float halfW = width * 0.5f;

    float nx = 0.0f, ny = 0.0f, nz = -1.0f; // normal for front wall

    // Door frame
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorFrameTex);

    glBegin(GL_QUADS);

    // Left frame
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x - halfW - 0.1f, yBottom, z + 0.01f);
    glTexCoord2f(1, 0);
    glVertex3f(x - halfW, yBottom, z + 0.01f);
    glTexCoord2f(1, 1);
    glVertex3f(x - halfW, yTop, z + 0.01f);
    glTexCoord2f(0, 1);
    glVertex3f(x - halfW - 0.1f, yTop, z + 0.01f);

    // Right frame
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x + halfW, yBottom, z + 0.01f);
    glTexCoord2f(1, 0);
    glVertex3f(x + halfW + 0.1f, yBottom, z + 0.01f);
    glTexCoord2f(1, 1);
    glVertex3f(x + halfW + 0.1f, yTop, z + 0.01f);
    glTexCoord2f(0, 1);
    glVertex3f(x + halfW, yTop, z + 0.01f);

    // Top frame
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x - halfW - 0.1f, yTop, z + 0.01f);
    glTexCoord2f(1, 0);
    glVertex3f(x + halfW + 0.1f, yTop, z + 0.01f);
    glTexCoord2f(1, 1);
    glVertex3f(x + halfW + 0.1f, yTop + 0.1f, z + 0.01f);
    glTexCoord2f(0, 1);
    glVertex3f(x - halfW - 0.1f, yTop + 0.1f, z + 0.01f);

    glEnd();

    // Door panel
    glBindTexture(GL_TEXTURE_2D, doorFrameTex);
    glBegin(GL_QUADS);

    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x - halfW, yBottom, z);
    glTexCoord2f(1, 0);
    glVertex3f(x + halfW, yBottom, z);
    glTexCoord2f(1, 1);
    glVertex3f(x + halfW, yTop, z);
    glTexCoord2f(0, 1);
    glVertex3f(x - halfW, yTop, z);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Door knob
    glPushMatrix();
    glTranslatef(x + halfW - 0.3f, yBottom + height * 0.5f, z + 0.05f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorKnobTex);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    drawSphere(0.12f, 20, 20);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Curved LED screen on wall
void drawCurvedScreen(float wallX, float wallZ, float width, float height,
                      float yBase, float radiusH, float radiusV, float zOffset)
{
    int segH = 64; // horizontal segments
    int segV = 32; // vertical segments

    float halfH = height * 0.5f;
    float angleH = width / radiusH;  // horizontal angle span
    float angleV = height / radiusV; // vertical angle span

    glPushMatrix();
    glTranslatef(wallX, yBase + halfH, wallZ);

    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, screenTex);

    glBegin(GL_QUADS);

    for (int i = 0; i < segH; i++)
    {
        float u1 = (float)i / segH;
        float u2 = (float)(i + 1) / segH;

        float t1 = -angleH * 0.5f + angleH * u1;
        float t2 = -angleH * 0.5f + angleH * u2;

        for (int j = 0; j < segV; j++)
        {
            float v1 = (float)j / segV;
            float v2 = (float)(j + 1) / segV;

            float p1 = -angleV * 0.5f + angleV * v1;
            float p2 = -angleV * 0.5f + angleV * v2;

            // Precompute trig values
            float sin_t1 = sinf(t1), cos_t1 = cosf(t1);
            float sin_t2 = sinf(t2), cos_t2 = cosf(t2);

            float sin_p1 = sinf(p1), sin_p2 = sinf(p2);

            // Vertex positions
            float x11 = radiusH * sin_t1;
            float z11 = -radiusH * cos_t1 + radiusH + zOffset;
            float y11 = radiusV * sin_p1;

            float x21 = radiusH * sin_t2;
            float z21 = -radiusH * cos_t2 + radiusH + zOffset;
            float y21 = radiusV * sin_p1;

            float x22 = x21;
            float z22 = z21;
            float y22 = radiusV * sin_p2;

            float x12 = x11;
            float z12 = z11;
            float y12 = radiusV * sin_p2;

            // Normals
            float nx_t1 = sin_t1, nz_t1 = cos_t1;
            float nx_t2 = sin_t2, nz_t2 = cos_t2;

            // Quad vertices
            glNormal3f(nx_t1, sin_p1, nz_t1);
            glTexCoord2f(u1, v1);
            glVertex3f(x11, y11, z11);

            glNormal3f(nx_t2, sin_p1, nz_t2);
            glTexCoord2f(u2, v1);
            glVertex3f(x21, y21, z21);

            glNormal3f(nx_t2, sin_p2, nz_t2);
            glTexCoord2f(u2, v2);
            glVertex3f(x22, y22, z22);

            glNormal3f(nx_t1, sin_p2, nz_t1);
            glTexCoord2f(u1, v2);
            glVertex3f(x12, y12, z12);
        }
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawLamp(float xPos, float zPos)
{
    int segments = 32;

    float baseRadius = 0.25f;
    float baseHeight = 0.05f;
    float poleRadius = 0.03f;
    float poleHeight = 5.0f;
    float shadeBottomRadius = 0.2f;
    float shadeTopRadius = 0.5f;
    float shadeHeight = 0.6f;

    glPushMatrix();
    glTranslatef(xPos, baseHeight, zPos);

    // Base disk
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lampRodTex);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i <= segments; i++)
    {
        float t = 2.0f * M_PI * i / segments;
        float u = (cosf(t) + 1.0f) * 0.5f;
        float v = (sinf(t) + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(baseRadius * cosf(t), 0.0f, baseRadius * sinf(t));
    }
    glEnd();

    // Pole cylinder
    for (int i = 0; i < segments; i++)
    {
        float t1 = 2.0f * M_PI * i / segments;
        float t2 = 2.0f * M_PI * (i + 1) / segments;

        float x1 = poleRadius * cosf(t1);
        float z1 = poleRadius * sinf(t1);
        float x2 = poleRadius * cosf(t2);
        float z2 = poleRadius * sinf(t2);

        float nx1 = cosf(t1), nz1 = sinf(t1);
        float nx2 = cosf(t2), nz2 = sinf(t2);

        glBegin(GL_QUADS);

        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)i / segments, 0.0f);
        glVertex3f(x1, 0.0f, z1);

        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(i + 1) / segments, 0.0f);
        glVertex3f(x2, 0.0f, z2);

        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(i + 1) / segments, 1.0f);
        glVertex3f(x2, poleHeight, z2);

        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)i / segments, 1.0f);
        glVertex3f(x1, poleHeight, z1);

        glEnd();
    }

    // Transparent shade mode
    if (lightState == 2)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthMask(GL_FALSE);     // allow bulb to show through
        glColor4f(1, 1, 1, 0.35f); // 35% opaque
    }
    else
    {
        glDisable(GL_BLEND);
        glColor4f(1, 1, 1, 1);
    }

    // Shade sides (truncated cone)
    glBindTexture(GL_TEXTURE_2D, lampShadeTex);

    float shadeBottomY = poleHeight;
    float shadeTopY = poleHeight + shadeHeight;

    for (int i = 0; i < segments; i++)
    {
        float t1 = 2.0f * M_PI * i / segments;
        float t2 = 2.0f * M_PI * (i + 1) / segments;

        float x1b = shadeBottomRadius * cosf(t1);
        float z1b = shadeBottomRadius * sinf(t1);
        float x2b = shadeBottomRadius * cosf(t2);
        float z2b = shadeBottomRadius * sinf(t2);

        float x1t = shadeTopRadius * cosf(t1);
        float z1t = shadeTopRadius * sinf(t1);
        float x2t = shadeTopRadius * cosf(t2);
        float z2t = shadeTopRadius * sinf(t2);

        float nx1 = cosf(t1), nz1 = sinf(t1);
        float nx2 = cosf(t2), nz2 = sinf(t2);

        glBegin(GL_QUADS);

        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)i / segments, 0.0f);
        glVertex3f(x1b, shadeBottomY, z1b);

        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(i + 1) / segments, 0.0f);
        glVertex3f(x2b, shadeBottomY, z2b);

        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(i + 1) / segments, 1.0f);
        glVertex3f(x2t, shadeTopY, z2t);

        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)i / segments, 1.0f);
        glVertex3f(x1t, shadeTopY, z1t);

        glEnd();
    }

    // reset state after transparent shade
    if (lightState == 2)
    {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    glDisable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);

    // light bulb
    glPushMatrix();

    float bulbLocalY = shadeBottomY + 0.4f; // inside shade
    glTranslatef(0.0f, bulbLocalY, 0.0f);

    // Default bulb color
    glColor3f(1.0f, 1.0f, 0.8f);

    // Glow only when lightState == 2
    if (lightState == 2)
    {
        float glow[] = {0.9f, 0.8f, 0.5f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, glow);
    }
    else
    {
        float noGlow[] = {0, 0, 0, 1};
        glMaterialfv(GL_FRONT, GL_EMISSION, noGlow);
    }

    drawSphere(0.25f, 20, 20); // bulb

    glPopMatrix();

    // set light position if lamp is on
    if (lightState == 2)
    {
        float bulbWorldX = xPos;
        float bulbWorldY = baseHeight + bulbLocalY;
        float bulbWorldZ = zPos;

        float pos1[] = {bulbWorldX, bulbWorldY, bulbWorldZ, 1.0f};
        glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    }

    glPopMatrix();
}

// draws one banquet chair at world position (x,z)
void drawBanquetChair(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // seat params
    const float seatY = 1.0f;
    const float seatT = 0.25f;
    const float seatW = 1.0f;
    const float seatD = 1.0f;

    // leg params
    const float legH = 1.0f;
    const float legW = 0.07f;

    // backrest params
    const float backH = 1.3f;
    const float curveD = 0.25f;
    const int   segs = 20;

    const float halfT = seatT * 0.5f;
    const float backBaseY = seatY - halfT;
    const float baseZ = -seatD * 0.5f;

    glEnable(GL_TEXTURE_2D);

    // ---------------------------------------
    // seat block (6 faces)
    // ---------------------------------------
    glBindTexture(GL_TEXTURE_2D, chairCushionTex);

    glBegin(GL_QUADS);
    float yTop = seatY + halfT;
    float yBot = seatY - halfT;

    // top
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yTop, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yTop, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yTop,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yTop,  seatD/2);

    // bottom
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yBot,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yBot,  seatD/2);

    // left
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,0); glVertex3f(-seatW/2, yTop, -seatD/2);
    glTexCoord2f(1,1); glVertex3f(-seatW/2, yTop,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yBot,  seatD/2);

    // right
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f(seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,0); glVertex3f(seatW/2, yTop, -seatD/2);
    glTexCoord2f(1,1); glVertex3f(seatW/2, yTop,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(seatW/2, yBot,  seatD/2);

    // front
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yBot, seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yBot, seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yTop, seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yTop, seatD/2);

    // back
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yBot, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yTop, -seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yTop, -seatD/2);

    glEnd();

    // ---------------------------------------
    // chair legs (4 repeats)
    // ---------------------------------------
    glBindTexture(GL_TEXTURE_2D, chairLegTex);

    float legX = seatW/2 - 0.1f;
    float legZ = seatD/2 - 0.1f;

    for (int sx = -1; sx <= 1; sx += 2)      // left/right
    for (int sz = -1; sz <= 1; sz += 2)      // front/back
    {
        float lx = sx * legX;
        float lz = sz * legZ;

        glBegin(GL_QUADS);
        float lb = yBot - legH;

        // front face
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(lx - legW/2, lb, lz - legW/2);
        glTexCoord2f(1,0); glVertex3f(lx + legW/2, lb, lz - legW/2);
        glTexCoord2f(1,1); glVertex3f(lx + legW/2, yBot, lz - legW/2);
        glTexCoord2f(0,1); glVertex3f(lx - legW/2, yBot, lz - legW/2);

        // back
        glNormal3f(0,0,-1);
        glTexCoord2f(0,0); glVertex3f(lx - legW/2, lb, lz + legW/2);
        glTexCoord2f(1,0); glVertex3f(lx + legW/2, lb, lz + legW/2);
        glTexCoord2f(1,1); glVertex3f(lx + legW/2, yBot, lz + legW/2);
        glTexCoord2f(0,1); glVertex3f(lx - legW/2, yBot, lz + legW/2);

        // left
        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(lx - legW/2, lb, lz - legW/2);
        glTexCoord2f(1,0); glVertex3f(lx - legW/2, lb, lz + legW/2);
        glTexCoord2f(1,1); glVertex3f(lx - legW/2, yBot, lz + legW/2);
        glTexCoord2f(0,1); glVertex3f(lx - legW/2, yBot, lz - legW/2);

        // right
        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(lx + legW/2, lb, lz - legW/2);
        glTexCoord2f(1,0); glVertex3f(lx + legW/2, lb, lz + legW/2);
        glTexCoord2f(1,1); glVertex3f(lx + legW/2, yBot, lz + legW/2);
        glTexCoord2f(0,1); glVertex3f(lx + legW/2, yBot, lz - legW/2);

        glEnd();
    }

    // ---------------------------------------
    // curved backrest
    // ---------------------------------------
    glBindTexture(GL_TEXTURE_2D, chairCushionTex);

    for (int i = 0; i < segs; i++)
    {
        float t1 = (float)i     / segs;
        float t2 = (float)(i+1) / segs;

        float y1 = backBaseY + t1 * backH;
        float y2 = backBaseY + t2 * backH;

        float z1 = baseZ - curveD * (t1*t1);
        float z2 = baseZ - curveD * (t2*t2);

        float dy = y2 - y1;
        float dz = z2 - z1;
        float len = sqrtf(dy*dy + dz*dz);

        float ny = dy/len;
        float nz = dz/len;

        // front surface
        glBegin(GL_QUADS);
        glNormal3f(0, ny, nz);
        glTexCoord2f(0,t1); glVertex3f(-seatW/2, y1, z1 + halfT);
        glTexCoord2f(1,t1); glVertex3f( seatW/2, y1, z1 + halfT);
        glTexCoord2f(1,t2); glVertex3f( seatW/2, y2, z2 + halfT);
        glTexCoord2f(0,t2); glVertex3f(-seatW/2, y2, z2 + halfT);
        glEnd();

        // back surface
        glBegin(GL_QUADS);
        glNormal3f(0, ny,-nz);
        glTexCoord2f(0,t1); glVertex3f(-seatW/2, y1, z1 - halfT);
        glTexCoord2f(1,t1); glVertex3f( seatW/2, y1, z1 - halfT);
        glTexCoord2f(1,t2); glVertex3f( seatW/2, y2, z2 - halfT);
        glTexCoord2f(0,t2); glVertex3f(-seatW/2, y2, z2 - halfT);
        glEnd();

        // left edge
        glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glTexCoord2f(0,t1); glVertex3f(-seatW/2, y1, z1 - halfT);
        glTexCoord2f(1,t1); glVertex3f(-seatW/2, y1, z1 + halfT);
        glTexCoord2f(1,t2); glVertex3f(-seatW/2, y2, z2 + halfT);
        glTexCoord2f(0,t2); glVertex3f(-seatW/2, y2, z2 - halfT);
        glEnd();

        // right edge
        glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glTexCoord2f(0,t1); glVertex3f(seatW/2, y1, z1 - halfT);
        glTexCoord2f(1,t1); glVertex3f(seatW/2, y1, z1 + halfT);
        glTexCoord2f(1,t2); glVertex3f(seatW/2, y2, z2 + halfT);
        glTexCoord2f(0,t2); glVertex3f(seatW/2, y2, z2 - halfT);
        glEnd();
    }

    // backrest top cap
    float yCap = backBaseY + backH;
    float zCap = baseZ - curveD;

    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yCap, zCap - halfT);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yCap, zCap - halfT);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yCap, zCap + halfT);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yCap, zCap + halfT);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


// Snowflake cocktail table
void drawCocktailTable2(float x, float z)
{
    const int N = 8;
    const int ARC_STEPS = 12;
    const float R = 1.4f;
    const float CURVE_IN = 0.22f;
    const float THICK = 0.12f;

    const float HEIGHT = 4.0f;
    const float LEG_H = HEIGHT;
    const float LEG_TOP_R = 0.10f;
    const float LEG_BOTTOM_R = 0.04f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    // Tabletop texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2Tex);

    float wedgeAngle = 2.0f * M_PI / N;

    // Top face
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float a0 = w * wedgeAngle;
        float a1 = (w + 1) * wedgeAngle;

        float prevX = 0.0f, prevZ = 0.0f;

        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            float mid = fabsf(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1.0f - mid);
            float r = R - curve;

            float xArc = r * cosf(a);
            float zArc = r * sinf(a);

            if (i > 0)
            {
                glNormal3f(0, 1, 0);
                glTexCoord2f(0.5f, 0.5f);
                glVertex3f(0, HEIGHT + THICK, 0);

                glTexCoord2f((cosf(a0) + 1) * 0.5f, (sinf(a0) + 1) * 0.5f);
                glVertex3f(prevX, HEIGHT + THICK, prevZ);

                glTexCoord2f((cosf(a) + 1) * 0.5f, (sinf(a) + 1) * 0.5f);
                glVertex3f(xArc, HEIGHT + THICK, zArc);
            }

            prevX = xArc;
            prevZ = zArc;
        }
    }
    glEnd();

    // Side wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N * ARC_STEPS; i++)
    {
        float a = (float)i / (N * ARC_STEPS) * (2.0f * M_PI);
        float wedgePos = fmodf(a, wedgeAngle) / wedgeAngle;

        float mid = fabsf(wedgePos - 0.5f) * 2.0f;
        float curve = CURVE_IN * (1.0f - mid);

        float r = R - curve;

        float xSide = r * cosf(a);
        float zSide = r * sinf(a);

        glNormal3f(cosf(a), 0, sinf(a));

        float u = (float)i / (N * ARC_STEPS);
        glTexCoord2f(u, 1);
        glVertex3f(xSide, HEIGHT, zSide);
        glTexCoord2f(u, 0);
        glVertex3f(xSide, HEIGHT + THICK, zSide);
    }
    glEnd();

    // Underside
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float a0 = w * wedgeAngle;
        float a1 = (w + 1) * wedgeAngle;

        float prevX = 0.0f, prevZ = 0.0f;

        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            float mid = fabsf(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1.0f - mid);
            float r = R - curve;

            float xArc = r * cosf(a);
            float zArc = r * sinf(a);

            if (i > 0)
            {
                glNormal3f(0, -1, 0);
                glVertex3f(0, HEIGHT, 0);
                glVertex3f(prevX, HEIGHT, prevZ);
                glVertex3f(xArc, HEIGHT, zArc);
            }

            prevX = xArc;
            prevZ = zArc;
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Leg texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2LegTex);

    // Three legs
    for (int i = 0; i < 3; i++)
    {
        float ang = i * 2.0f * M_PI / 3.0f;
        float lx = 0.8f * cosf(ang);
        float lz = 0.8f * sinf(ang);

        glPushMatrix();
        glTranslatef(lx, HEIGHT, lz);
        glRotatef(-10, sinf(ang), 0, -cosf(ang));

        glBegin(GL_QUAD_STRIP);
        for (int s = 0; s <= 24; s++)
        {
            float t = 2.0f * M_PI * s / 24.0f;
            float nx = cosf(t);
            float nz = sinf(t);
            float u = (float)s / 24.0f;

            glNormal3f(nx, 0, nz);

            glTexCoord2f(u, 1);
            glVertex3f(nx * LEG_BOTTOM_R, -LEG_H, nz * LEG_BOTTOM_R);

            glTexCoord2f(u, 0);
            glVertex3f(nx * LEG_TOP_R, 0, nz * LEG_TOP_R);
        }
        glEnd();

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Draw the cocktail table model
void drawCocktailTable3(float x, float z)
{
    int slices = 40;
    float bottomHeight = 2.3f;
    float topHeight = 1.7f;
    float R = 1.35f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    // Leg texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3LegTex);

    // Bottom frustum
    drawFrustum(0.85f, 0.45f, bottomHeight, slices);

    glTranslatef(0, bottomHeight, 0);
    glDisable(GL_TEXTURE_2D);

    // Upper frustum
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);
    drawFrustum(0.45f, 1.35f, topHeight, slices);
    glDisable(GL_TEXTURE_2D);

    // Tabletop texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);

    // Top disk
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topHeight, 0);

    for (int i = 0; i <= slices; i++)
    {
        float ang = 2.0f * M_PI * i / slices;
        float cx = cosf(ang);
        float cz = sinf(ang);

        glTexCoord2f((cx + 1) * 0.5f, (cz + 1) * 0.5f);
        glVertex3f(R * cx, topHeight, R * cz);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawMeetingTable(float x, float z)
{
    float length = 8.0f;
    float width = 2.6f;

    float tabletopY = 1.7f;
    float thickness = 0.15f;
    float legHeight = 1.6f;
    float legSize = 0.25f;

    int segments = 140;

    float halfL = length * 0.5f;
    float halfW = width * 0.5f;

    // Draw legs
    glPushMatrix();
    glTranslatef(x, legHeight * 0.5f, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, meetingTableLegTex);
    glColor3f(1, 1, 1);

    float lx = halfL * 0.55f;
    float lz = halfW * 0.45f;

    glPushMatrix();
    glTranslatef(lx, 0, lz);
    drawCuboid(legSize, legHeight, legSize);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-lx, 0, lz);
    drawCuboid(legSize, legHeight, legSize);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(lx, 0, -lz);
    drawCuboid(legSize, legHeight, legSize);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-lx, 0, -lz);
    drawCuboid(legSize, legHeight, legSize);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Tabletop
    glPushMatrix();
    glTranslatef(x, tabletopY, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, meetingTableTex);
    glColor3f(1, 1, 1);

    // Precompute tabletop shape
    static float px[200], pz[200];
    int N = segments;
    if (N > 200)
        N = 200; // safety check

    for (int i = 0; i < N; i++)
    {
        float t = (float)i / (N - 1);
        float angle = t * 2.0f * M_PI;

        float nx = cosf(angle);
        float ny = sinf(angle);

        float rx = halfL * (0.7f + 0.3f * fabsf(ny));
        float ry = halfW;

        float bulge = 1.0f + 0.20f * expf(-powf((fabsf(nx) - 0.3f) * 5.0f, 2.0f));
        float pinch = 1.0f - 0.08f * expf(-powf(nx * 4.0f, 2.0f));

        float finalWidth = ry * bulge * pinch;

        px[i] = rx * nx;
        pz[i] = finalWidth * ny;
    }

    // Top surface (triangle fan keeps highlight pass in hardware)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= N; i++)
    {
        int k = i % N;
        glTexCoord2f((px[k] / length) + 0.5f, (pz[k] / width) + 0.5f);
        glVertex3f(px[k], 0, pz[k]);
    }
    glEnd();

    // Bottom surface
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, -thickness, 0.0f);
    for (int i = 0; i <= N; i++)
    {
        int k = i % N;
        glTexCoord2f((px[k] / length) + 0.5f, (pz[k] / width) + 0.5f);
        glVertex3f(px[k], -thickness, pz[k]);
    }
    glEnd();

    // Side walls
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N; i++)
    {
        int k = i % N;
        float sx = px[k];
        float sz = pz[k];
        float len = sqrtf(sx * sx + sz * sz);

        float nx = sx / len;
        float nz = sz / len;

        glNormal3f(nx, 0, nz);

        float u = (float)i / N;

        glTexCoord2f(u, 0);
        glVertex3f(sx, 0, sz);
        glTexCoord2f(u, 1);
        glVertex3f(sx, -thickness, sz);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Rope for ceiling light
static void drawRope(float length)
{
    glColor3f(1, 1, 1); // keep rope texture untinted

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, threadTex);

    glTranslatef(0, -length / 2.0f, 0);
    glScalef(0.05f, length, 0.05f);

    drawCuboid(1,1,1);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawStarShape()
{
    const float outerR = 1.0f;
    const float innerR = 0.55f;
    const float depth = 0.35f;
    const int numPoints = 5;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, starTex);
    glColor3f(1, 1, 1); // show texture fully

    // Front face (+Z)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, 1);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, depth);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cosf(ang);
        float y = r * sinf(ang);

        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, depth);
    }
    glEnd();

    // Back face (–Z)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, -1);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, -depth);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cosf(ang);
        float y = r * sinf(ang);

        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, -depth);
    }
    glEnd();

    // Side walls
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cosf(ang);
        float y = r * sinf(ang);

        float len = sqrtf(x * x + y * y);
        glNormal3f(x / len, y / len, 0);

        float u = (float)i / (numPoints * 2);

        glTexCoord2f(u, 0);
        glVertex3f(x, y, depth);
        glTexCoord2f(u, 1);
        glVertex3f(x, y, -depth);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawCloudShape()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTex);
    glColor3f(1, 1, 1); // show texture fully

    // Center puff
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(1.4f, 1.2f, 1.0f);
    drawSphere(0.5f, 32, 32);
    glPopMatrix();

    // Right big puff
    glPushMatrix();
    glTranslatef(0.65f, -0.05f, 0.0f);
    glScalef(1.2f, 1.0f, 1.0f);
    drawSphere(0.45f, 32, 32);
    glPopMatrix();

    // Left big puff
    glPushMatrix();
    glTranslatef(-0.65f, -0.05f, 0.0f);
    glScalef(1.2f, 1.0f, 1.0f);
    drawSphere(0.45f, 32, 32);
    glPopMatrix();

    // Lower-left puff
    glPushMatrix();
    glTranslatef(-0.35f, -0.35f, 0.0f);
    glScalef(0.9f, 0.8f, 1.0f);
    drawSphere(0.35f, 32, 32);
    glPopMatrix();

    // Lower-right puff
    glPushMatrix();
    glTranslatef(0.35f, -0.35f, 0.0f);
    glScalef(0.9f, 0.8f, 1.0f);
    drawSphere(0.35f, 32, 32);
    glPopMatrix();

    // Top-right puff
    glPushMatrix();
    glTranslatef(0.30f, 0.35f, 0.0f);
    glScalef(0.8f, 0.8f, 1.0f);
    drawSphere(0.30f, 32, 32);
    glPopMatrix();

    // Top-left puff
    glPushMatrix();
    glTranslatef(-0.30f, 0.35f, 0.0f);
    glScalef(0.8f, 0.8f, 1.0f);
    drawSphere(0.30f, 32, 32);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

static void drawMoonShape()
{
    float outerR = 0.9f;
    float innerR = 0.60f;
    float offset = 0.35f;
    float depth = 0.15f;
    int steps = 300;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, moonTex);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();

    // Front face
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        // Outer point must lie outside inner arc
        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float u = (cos(a) + 1) * 0.5f;
            float v = (sin(a) + 1) * 0.5f;

            glTexCoord2f(u, v);
            glVertex3f(ox, oy, depth);

            glTexCoord2f(u, v);
            glVertex3f(ix, iy, depth);
        }
    }

    glEnd();

    // Back face
    glNormal3f(0, 0, -1);
    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float u = (cos(a) + 1) * 0.5f;
            float v = (sin(a) + 1) * 0.5f;

            glTexCoord2f(u, v);
            glVertex3f(ox, oy, -depth);

            glTexCoord2f(u, v);
            glVertex3f(ix, iy, -depth);
        }
    }

    glEnd();

    // Outer walls
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float len = sqrtf(ox * ox + oy * oy);
            glNormal3f(ox / len, oy / len, 0);

            float u = (float)i / steps; // wrap texture around edge
            float v1 = 0.0f;
            float v2 = 1.0f;

            glTexCoord2f(u, v1);
            glVertex3f(ox, oy, depth);
            glTexCoord2f(u, v2);
            glVertex3f(ox, oy, -depth);
        }
    }

    glEnd();

    // Inner walls
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 2.0f * M_PI;

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float len = sqrtf((ix - offset) * (ix - offset) + iy * iy);
            float nx = -(ix - offset) / len;
            float ny = -iy / len;
            glNormal3f(nx, ny, 0);

            float u = (float)i / steps;
            float v1 = 0.0f;
            float v2 = 1.0f;

            glTexCoord2f(u, v1);
            glVertex3f(ix, iy, -depth);
            glTexCoord2f(u, v2);
            glVertex3f(ix, iy, depth);
        }
    }

    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawCeilingLight(float x, float y, float z, int shapeType)
{
    float ropeLength = 2.0f;
    float shapeOffset = ropeLength + 0.45f;

    glPushMatrix();
    glTranslatef(x, y, z);

    // Rope
    drawRope(ropeLength);

    // Shape offset
    glPushMatrix();
    glTranslatef(0, -shapeOffset, 0);

    // Select shape
    switch (shapeType)
    {
    case 0:
        drawStarShape();
        break;
    case 1:
        drawCloudShape();
        break;
    case 2:
        drawMoonShape();
        break;
    default:
        break;
    }

    glPopMatrix();
    glPopMatrix();
}

void drawCeilingLights()
{
    // Row 1
    drawCeilingLight(-10, 14.5f, -10, 0);
    drawCeilingLight(0, 14.5f, -10, 1);
    drawCeilingLight(10, 14.5f, -10, 2);

    // Row 2
    drawCeilingLight(-10, 14.5f, 0, 1);
    drawCeilingLight(0, 14.5f, 0, 2);
    drawCeilingLight(10, 14.5f, 0, 0);

    // Row 3
    drawCeilingLight(-10, 14.5f, 10, 2);
    drawCeilingLight(0, 14.5f, 10, 0);
    drawCeilingLight(10, 14.5f, 10, 1);
}

// Dimensions
float CHAIR_SCALE = 1.8f;

float BASE_THICK;
float CUSHION_THICK;
float LEG_HEIGHT;
float SEAT_HEIGHT;
float ROD_HEIGHT;
float ROD_BOTTOM;
float ROD_TOP;

float LEG_OFFSET_X = 0.45f;
float LEG_OFFSET_Z = 0.35f;

void drawSeat()
{
    float width = 1.0f;
    float halfW = width * 0.5f;
    float rectDepth = 0.55f;
    float radius = halfW;
    int steps = 48;
    float h = CUSHION_THICK;

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairCushionTex);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Position the seat
    float bottomY = SEAT_HEIGHT - h;
    glTranslatef(0.0f, bottomY + h * 0.5f, 0.0f);

    // Top face
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, h / 2, rectDepth);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, h / 2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = M_PI * t;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);

        glTexCoord2f(t, 0);
        glVertex3f(x, h / 2, z);
    }
    glEnd();

    // Bottom face
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, -h / 2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = M_PI * t;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);

        glTexCoord2f(t, 0);
        glVertex3f(x, -h / 2, z);
    }
    glEnd();

    // Front wall
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 0);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glEnd();

    // Curved back wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = M_PI * t;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);

        glTexCoord2f(t, 0);
        glVertex3f(x, -h / 2, z);
        glTexCoord2f(t, 1);
        glVertex3f(x, +h / 2, z);
    }
    glEnd();

    // Left wall
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 0);
    glVertex3f(-halfW, -h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(-halfW, +h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glEnd();

    // Right wall
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 0);
    glVertex3f(+halfW, -h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, +h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Seat base under the cushion
void drawSeatBase()
{
    glPushMatrix();
    glColor3f(0.55f, 0.38f, 0.22f); // darker wood

    // dimensions
    const float width = 1.12f;
    const float halfW = width * 0.5f;
    const float rectDepth = 0.62f;
    const float radius = halfW;
    const int steps = 48;
    const float h = BASE_THICK;

    // position base under cushion
    float bottomY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;
    glTranslatef(0.0f, bottomY + h * 0.5f, 0.0f);

    // top face
    glBegin(GL_POLYGON);
    glVertex3f(-halfW, h / 2, rectDepth);
    glVertex3f(+halfW, h / 2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float a = M_PI * (float)i / (float)steps;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);
        glVertex3f(x, h / 2, z);
    }
    glEnd();

    // bottom face
    glBegin(GL_POLYGON);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, -h / 2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float a = M_PI * (float)i / (float)steps;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);
        glVertex3f(x, -h / 2, z);
    }
    glEnd();

    // front wall
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glEnd();

    // curved back wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float a = M_PI * (float)i / (float)steps;
        float x = radius * cosf(a);
        float z = -radius * sinf(a);

        glVertex3f(x, -h / 2, z);
        glVertex3f(x, +h / 2, z);
    }
    glEnd();

    // left side
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, 0);
    glVertex3f(-halfW, -h / 2, 0);
    glEnd();

    // right side
    glBegin(GL_QUADS);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, 0);
    glVertex3f(+halfW, -h / 2, 0);
    glEnd();

    glPopMatrix();
}

// Single leg at position (x, z)
void drawLegN(float x, float z)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    float legStartY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;
    glTranslatef(x, legStartY - LEG_HEIGHT * 0.5f, z);
    glScalef(0.12f, LEG_HEIGHT, 0.12f);

    drawCuboid(1,1,1);
    glPopMatrix();
}

// Horizontal bar between two legs
void drawFootBar(float x1, float z1, float x2, float z2, float y)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    glTranslatef((x1 + x2) * 0.5f, y, (z1 + z2) * 0.5f);

    float dx = x2 - x1;
    float dz = z2 - z1;
    float len = sqrtf(dx * dx + dz * dz);

    float ang = atan2f(dx, dz) * (180.0f / M_PI); // orientation around Y
    glRotatef(ang, 0, 1, 0);

    glScalef(len, 0.08f, 0.08f);
    drawCuboid(1,1,1);

    glPopMatrix();
}

// Curved wooden backrest
void drawCurvedBackrest()
{
    float rodBottom = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    float rodTop = rodBottom + (0.75f * CHAIR_SCALE);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairBackTex);
    glColor3f(1, 1, 1);

    glTranslatef(0.0f, rodTop, -0.1f);
    glRotatef(-10, 1, 0, 0);
    glRotatef(180, 0, 1, 0);

    const float radius = 0.65f;
    const float height = 0.35f;
    const float thickness = 0.30f;
    const int steps = 160;

    for (int j = 0; j < steps; j++)
    {
        float t0 = (float)j / steps;
        float t1 = (float)(j + 1) / steps;

        float a1 = ((j - steps / 2) * (M_PI / 180.0f));
        float a2 = ((j - steps / 2 + 1) * (M_PI / 180.0f));

        float x1 = radius * sinf(a1);
        float z1 = radius * cosf(a1);
        float x2 = radius * sinf(a2);
        float z2 = radius * cosf(a2);

        glBegin(GL_QUADS);

        // front plate
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1);

        // back plate
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1 - thickness);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2 - thickness);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1 - thickness);

        // top edge
        glTexCoord2f(t0, 1);
        glVertex3f(x1, height, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, height, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1 - thickness);

        // bottom edge
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, 0, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, 0, z1 - thickness);

        // left side
        if (j == 0)
        {
            glTexCoord2f(0, 1);
            glVertex3f(x1, 0, z1);
            glTexCoord2f(1, 1);
            glVertex3f(x1, 0, z1 - thickness);
            glTexCoord2f(1, 0);
            glVertex3f(x1, height, z1 - thickness);
            glTexCoord2f(0, 0);
            glVertex3f(x1, height, z1);
        }

        // right side
        if (j == steps - 1)
        {
            glTexCoord2f(0, 1);
            glVertex3f(x2, 0, z2);
            glTexCoord2f(1, 1);
            glVertex3f(x2, 0, z2 - thickness);
            glTexCoord2f(1, 0);
            glVertex3f(x2, height, z2 - thickness);
            glTexCoord2f(0, 0);
            glVertex3f(x2, height, z2);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Vertical rod for backrest
void drawBackrestRod(float x, float z)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    glTranslatef(x, ROD_BOTTOM + ROD_HEIGHT * 0.5f, z);
    glScalef(0.08f, ROD_HEIGHT, 0.08f);

    drawCuboid(1,1,1);
    glPopMatrix();
}

// Full bar chair assembly
void drawBarChair()
{
    BASE_THICK = 0.15f * CHAIR_SCALE;
    CUSHION_THICK = 0.18f * CHAIR_SCALE;
    LEG_HEIGHT = 1.2f * CHAIR_SCALE;

    ROD_HEIGHT = 0.75f * CHAIR_SCALE;
    ROD_BOTTOM = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    ROD_TOP = ROD_BOTTOM + ROD_HEIGHT;

    SEAT_HEIGHT = LEG_HEIGHT + BASE_THICK + CUSHION_THICK;

    glPushMatrix();

    // seat base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    drawSeatBase();
    glDisable(GL_TEXTURE_2D);

    // cushion
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairCushionTex);
    drawSeat();
    glDisable(GL_TEXTURE_2D);

    // legs
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    drawLegN(+LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(+LEG_OFFSET_X, -LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, -LEG_OFFSET_Z);
    glDisable(GL_TEXTURE_2D);

    // backrest rods
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    drawBackrestRod(0.0f, -0.50f);
    drawBackrestRod(0.20f, -0.50f);
    drawBackrestRod(-0.20f, -0.50f);
    glDisable(GL_TEXTURE_2D);

    // curved backrest
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairBackTex);
    drawCurvedBackrest();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Place chair in the world
void drawBarChairObj(float x, float z)
{
    glPushMatrix();

    float yOffset = 0.0f;

    glTranslatef(x, yOffset, z);
    glScalef(1.0f, 1.2f, 1.0f);

    drawBarChair();
    glPopMatrix();
}

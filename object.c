#include "CSCIx229.h"

// Draws a single table leg using a cylinder shape
static void drawLeg(float legPosX, float legPosZ, float legHeight, float legRadius)
{
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cocktailTableTex);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    glPushMatrix();
    glTranslatef(legPosX, 0.0f, legPosZ);

    drawCylinder(legRadius, legHeight, 32);

    glPopMatrix();
    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
}

// Draws the Event Table (Rectangular with rounded corners)
void drawTable(float posX, float posZ)
{
    glPushMatrix();
    glTranslatef(posX, 0, posZ);

    // Table Dimension
    float tabletopHeight = 2.0f;
    float tabletopThickness = 0.15f;
    float legHeight = tabletopHeight;
    float legThickness = 0.25f;
    float tabletopLength = 4.0f;
    float tabletopWidth = 2.0f;
    float cornerRadius = 0.3f;
    int cornerSegmentCount = 20;

    // Pre-calculate positions to make drawing easier
    float tabletopBottomY = tabletopHeight;
    float tabletopTopY = tabletopHeight + tabletopThickness;
    float tabletopHalfLength = tabletopLength * 0.5f;
    float tabletopHalfWidth = tabletopWidth * 0.5f;

    // The inner rectangle stops before the curved corners start
    float innerHalfLength = tabletopHalfLength - cornerRadius;
    float innerHalfWidth = tabletopHalfWidth - cornerRadius;

    // Apply table texture
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tableTex);
        glColor3f(1, 1, 1);
    }

    // Draw the main center rectangle
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-innerHalfLength, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glEnd();

    // Draw the flat strips on the sides (Front, Back, Left, Right)
    glBegin(GL_QUADS);

    // Front Strip
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, innerHalfWidth + cornerRadius);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, innerHalfWidth + cornerRadius);

    // Back Strip
    glTexCoord2f(0, 0);
    glVertex3f(-tabletopHalfLength, tabletopTopY, -innerHalfWidth - cornerRadius);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength, tabletopTopY, -innerHalfWidth - cornerRadius);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, -innerHalfWidth);

    // Left Strip
    glTexCoord2f(0, 0);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(-tabletopHalfLength, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopTopY, innerHalfWidth);

    // Right Strip
    glTexCoord2f(0, 0);
    glVertex3f(tabletopHalfLength, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopTopY, -innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopTopY, innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, innerHalfWidth);
    glEnd();

    // Draw the Rounded Corners (Top Surface)
    for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
    {
        // Determine center point of the current corner
        float cornerX = (cornerIndex == 0 || cornerIndex == 3) ? innerHalfLength : -innerHalfLength;
        float cornerZ = (cornerIndex < 2) ? innerHalfWidth : -innerHalfWidth;

        float startDeg = (float)(cornerIndex * 90);
        float endDeg = startDeg + 90.0f;

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 1, 0);

        // Center of the fan
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(cornerX, tabletopTopY, cornerZ);

        // Sweep around the arc
        for (int segmentIndex = 0; segmentIndex <= cornerSegmentCount; segmentIndex++)
        {
            float angleDegrees = startDeg + segmentIndex * (endDeg - startDeg) / cornerSegmentCount;
            float vertexX = cornerX + cornerRadius * Cos(angleDegrees);
            float vertexZ = cornerZ + cornerRadius * Sin(angleDegrees);

            // Map circular coordinates to texture coordinates so texture wraps nicely
            float texU = 0.5f + 0.5f * Cos(angleDegrees);
            float texV = 0.5f + 0.5f * Sin(angleDegrees);

            glTexCoord2f(texU, texV);
            glVertex3f(vertexX, tabletopTopY, vertexZ);
        }
        glEnd();
    }

    // Draw the Rounded Corners (Vertical Edges)
    for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
    {
        float cornerX = (cornerIndex == 0 || cornerIndex == 3) ? innerHalfLength : -innerHalfLength;
        float cornerZ = (cornerIndex < 2) ? innerHalfWidth : -innerHalfWidth;

        float startDeg = (float)(cornerIndex * 90);
        float endDeg = startDeg + 90.0f;

        glBegin(GL_QUAD_STRIP); // Connects top points to bottom points in a strip
        for (int segmentIndex = 0; segmentIndex <= cornerSegmentCount; segmentIndex++)
        {
            float angleDegrees = startDeg + segmentIndex * (endDeg - startDeg) / cornerSegmentCount;

            float vertexX = cornerX + cornerRadius * Cos(angleDegrees);
            float vertexZ = cornerZ + cornerRadius * Sin(angleDegrees);
            float normalX = Cos(angleDegrees);
            float normalZ = Sin(angleDegrees);

            glNormal3f(normalX, 0, normalZ);

            float texU = (float)segmentIndex / cornerSegmentCount;

            // Bottom vertex
            glTexCoord2f(texU, 0);
            glVertex3f(vertexX, tabletopBottomY, vertexZ);
            // Top vertex
            glTexCoord2f(texU, 1);
            glVertex3f(vertexX, tabletopTopY, vertexZ);
        }
        glEnd();
    }

    // Draw the flat side walls (Vertical Edges)
    glBegin(GL_QUADS);

    // Front Side
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-innerHalfLength, tabletopBottomY, innerHalfWidth + cornerRadius);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength, tabletopBottomY, innerHalfWidth + cornerRadius);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, innerHalfWidth + cornerRadius);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, innerHalfWidth + cornerRadius);

    // Back Side
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(-tabletopHalfLength, tabletopBottomY, -innerHalfWidth - cornerRadius);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength, tabletopBottomY, -innerHalfWidth - cornerRadius);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength, tabletopTopY, -innerHalfWidth - cornerRadius);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength, tabletopTopY, -innerHalfWidth - cornerRadius);

    // Left Side
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopBottomY, -innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopBottomY, innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopTopY, innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(-tabletopHalfLength - cornerRadius, tabletopTopY, -innerHalfWidth);

    // Right Side
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopBottomY, -innerHalfWidth);
    glTexCoord2f(1, 0);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopBottomY, innerHalfWidth);
    glTexCoord2f(1, 1);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopTopY, innerHalfWidth);
    glTexCoord2f(0, 1);
    glVertex3f(tabletopHalfLength + cornerRadius, tabletopTopY, -innerHalfWidth);

    glEnd();

    // Positions for 4 legs relative to center
    float legOffsets[4][2] = {
        {innerHalfLength, innerHalfWidth},
        {-innerHalfLength, innerHalfWidth},
        {-innerHalfLength, -innerHalfWidth},
        {innerHalfLength, -innerHalfWidth}};

    // Place four legs at the corners
    for (int legIndex = 0; legIndex < 4; legIndex++)
    {
        glPushMatrix();
        // Move to leg position
        glTranslatef(legOffsets[legIndex][0], legHeight * 0.5f, legOffsets[legIndex][1]);
        // Draw leg
        drawCuboid(legThickness, legHeight, legThickness);
        glPopMatrix();
    }

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

// Draws the simple round cocktail table
void drawCocktailTable(float x, float z)
{
    float height = 4.0f;
    float topRadius = 1.5f;
    float topThickness = 0.1f;
    float legRadius = 0.15f;

    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cocktailTableTex);
    }

    drawLeg(0.0f, 0.0f, height, legRadius);    // Center pole
    drawDisk(topRadius, height, topThickness); // Top circle
    drawDisk(0.6f, 0.0f, 0.05f);               // Base circle

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

// Draws a door
void drawDoor(float x, float z, float width, float height)
{
    float yBottom = 0.0f;
    float yTop = height;
    float halfW = width * 0.5f;

    float nx = 0.0f, ny = 0.0f, nz = -1.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorFrameTex);

    glBegin(GL_QUADS);

    // Left Frame
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x - halfW - 0.1f, yBottom, z + 0.01f);
    glTexCoord2f(1, 0);
    glVertex3f(x - halfW, yBottom, z + 0.01f);
    glTexCoord2f(1, 1);
    glVertex3f(x - halfW, yTop, z + 0.01f);
    glTexCoord2f(0, 1);
    glVertex3f(x - halfW - 0.1f, yTop, z + 0.01f);

    // Right Frame
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0);
    glVertex3f(x + halfW, yBottom, z + 0.01f);
    glTexCoord2f(1, 0);
    glVertex3f(x + halfW + 0.1f, yBottom, z + 0.01f);
    glTexCoord2f(1, 1);
    glVertex3f(x + halfW + 0.1f, yTop, z + 0.01f);
    glTexCoord2f(0, 1);
    glVertex3f(x + halfW, yTop, z + 0.01f);

    // Top Frame
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

    // Knob
    glPushMatrix();
    glTranslatef(x + halfW - 0.3f, yBottom + height * 0.5f, z + 0.05f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorKnobTex);

    // Make knob look shiny/reflective
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

// Draws a large curved screen
void drawCurvedScreen(float wallX, float wallZ, float width, float height,
                      float yBase, float radiusH, float radiusV, float zOffset)
{
    int horizontalSegmentCount = 64;
    int verticalSegmentCount = 32;

    float halfH = height * 0.5f;
    // Calculate how much angle the screen covers
    float angleHDeg = (width / radiusH) * (180.0f / PI);
    float angleVDeg = (height / radiusV) * (180.0f / PI);

    glPushMatrix();
    glTranslatef(wallX, yBase + halfH, wallZ);

    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, screenTex);

    glBegin(GL_QUADS);

    // Iterate over horizontal slices of the curved panel
    for (int horizontalIndex = 0; horizontalIndex < horizontalSegmentCount; horizontalIndex++)
    {
        float uStart = (float)horizontalIndex / horizontalSegmentCount;
        float uEnd = (float)(horizontalIndex + 1) / horizontalSegmentCount;

        float thetaStartDeg = -angleHDeg * 0.5f + angleHDeg * uStart;
        float thetaEndDeg = -angleHDeg * 0.5f + angleHDeg * uEnd;

        // For each slice, cover the vertical bands
        for (int verticalIndex = 0; verticalIndex < verticalSegmentCount; verticalIndex++)
        {
            float vStart = (float)verticalIndex / verticalSegmentCount;
            float vEnd = (float)(verticalIndex + 1) / verticalSegmentCount;

            float phiStartDeg = -angleVDeg * 0.5f + angleVDeg * vStart;
            float phiEndDeg = -angleVDeg * 0.5f + angleVDeg * vEnd;

            float sinThetaStart = Sin(thetaStartDeg), cosThetaStart = Cos(thetaStartDeg);
            float sinThetaEnd = Sin(thetaEndDeg), cosThetaEnd = Cos(thetaEndDeg);
            float sinPhiStart = Sin(phiStartDeg), sinPhiEnd = Sin(phiEndDeg);

            // Calculate corner positions for the quad
            float bottomLeftX = radiusH * sinThetaStart;
            float bottomLeftZ = -radiusH * cosThetaStart + radiusH + zOffset;
            float bottomLeftY = radiusV * sinPhiStart;

            float bottomRightX = radiusH * sinThetaEnd;
            float bottomRightZ = -radiusH * cosThetaEnd + radiusH + zOffset;
            float bottomRightY = radiusV * sinPhiStart;

            float topRightX = bottomRightX;
            float topRightZ = bottomRightZ;
            float topRightY = radiusV * sinPhiEnd;

            float topLeftX = bottomLeftX;
            float topLeftZ = bottomLeftZ;
            float topLeftY = radiusV * sinPhiEnd;

            // Normals point outwards from curve center
            float normalThetaStart = sinThetaStart, normalZThetaStart = cosThetaStart;
            float normalThetaEnd = sinThetaEnd, normalZThetaEnd = cosThetaEnd;

            // Draw the quad with normals and texture coords
            glNormal3f(normalThetaStart, sinPhiStart, normalZThetaStart);
            glTexCoord2f(uStart, vStart);
            glVertex3f(bottomLeftX, bottomLeftY, bottomLeftZ);

            glNormal3f(normalThetaEnd, sinPhiStart, normalZThetaEnd);
            glTexCoord2f(uEnd, vStart);
            glVertex3f(bottomRightX, bottomRightY, bottomRightZ);

            glNormal3f(normalThetaEnd, sinPhiEnd, normalZThetaEnd);
            glTexCoord2f(uEnd, vEnd);
            glVertex3f(topRightX, topRightY, topRightZ);

            glNormal3f(normalThetaStart, sinPhiEnd, normalZThetaStart);
            glTexCoord2f(uStart, vEnd);
            glVertex3f(topLeftX, topLeftY, topLeftZ);
        }
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Draws the standing floor lamp
void drawLamp(float xPos, float zPos)
{
    int segmentCount = 32;

    float baseRadius = 0.25f;
    float baseHeight = 0.05f;
    float poleRadius = 0.03f;
    float poleHeight = 5.0f;
    float shadeBottomRadius = 0.2f;
    float shadeTopRadius = 0.5f;
    float shadeHeight = 0.6f;

    glPushMatrix();
    glTranslatef(xPos, baseHeight, zPos);

    // Base Disk
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, lampRodTex);
        glColor3f(1, 1, 1);
    }

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    // Circle loop
    for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
    {
        float angleDegrees = 360.0f * segmentIndex / segmentCount;
        float u = (Cos(angleDegrees) + 1.0f) * 0.5f;
        float v = (Sin(angleDegrees) + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(baseRadius * Cos(angleDegrees), 0.0f, baseRadius * Sin(angleDegrees));
    }
    glEnd();

    // Pole
    // Connects bottom circle to top circle to form cylinder
    for (int segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++)
    {
        float angle1Deg = 360.0f * segmentIndex / segmentCount;
        float angle2Deg = 360.0f * (segmentIndex + 1) / segmentCount;

        float x1 = poleRadius * Cos(angle1Deg);
        float z1 = poleRadius * Sin(angle1Deg);
        float x2 = poleRadius * Cos(angle2Deg);
        float z2 = poleRadius * Sin(angle2Deg);

        float nx1 = Cos(angle1Deg), nz1 = Sin(angle1Deg);
        float nx2 = Cos(angle2Deg), nz2 = Sin(angle2Deg);

        glBegin(GL_QUADS);
        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)segmentIndex / segmentCount, 0.0f);
        glVertex3f(x1, 0.0f, z1);
        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(segmentIndex + 1) / segmentCount, 0.0f);
        glVertex3f(x2, 0.0f, z2);
        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(segmentIndex + 1) / segmentCount, 1.0f);
        glVertex3f(x2, poleHeight, z2);
        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)segmentIndex / segmentCount, 1.0f);
        glVertex3f(x1, poleHeight, z1);
        glEnd();
    }

    // Lamp Shade
    // If the lamp is ON (lightState == 2), make the shade semi-transparent
    if (glIsEnabled(GL_LIGHTING))
    {
        if (lightState == 2)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glColor4f(1, 1, 1, 0.35f); // 35% visible
        }
        else
        {
            glDisable(GL_BLEND);
            glColor4f(1, 1, 1, 1); // Opaque
        }
        glBindTexture(GL_TEXTURE_2D, lampShadeTex);
    }

    float shadeBottomY = poleHeight;
    float shadeTopY = poleHeight + shadeHeight;

    // Draw Cone (Trunctated) for Shade
    for (int segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++)
    {
        float angle1Deg = 360.0f * segmentIndex / segmentCount;
        float angle2Deg = 360.0f * (segmentIndex + 1) / segmentCount;

        float x1b = shadeBottomRadius * Cos(angle1Deg);
        float z1b = shadeBottomRadius * Sin(angle1Deg);
        float x2b = shadeBottomRadius * Cos(angle2Deg);
        float z2b = shadeBottomRadius * Sin(angle2Deg);

        float x1t = shadeTopRadius * Cos(angle1Deg);
        float z1t = shadeTopRadius * Sin(angle1Deg);
        float x2t = shadeTopRadius * Cos(angle2Deg);
        float z2t = shadeTopRadius * Sin(angle2Deg);

        float nx1 = Cos(angle1Deg), nz1 = Sin(angle1Deg);
        float nx2 = Cos(angle2Deg), nz2 = Sin(angle2Deg);

        glBegin(GL_QUADS);
        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)segmentIndex / segmentCount, 0.0f);
        glVertex3f(x1b, shadeBottomY, z1b);
        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(segmentIndex + 1) / segmentCount, 0.0f);
        glVertex3f(x2b, shadeBottomY, z2b);
        glNormal3f(nx2, 0.0f, nz2);
        glTexCoord2f((float)(segmentIndex + 1) / segmentCount, 1.0f);
        glVertex3f(x2t, shadeTopY, z2t);
        glNormal3f(nx1, 0.0f, nz1);
        glTexCoord2f((float)segmentIndex / segmentCount, 1.0f);
        glVertex3f(x1t, shadeTopY, z1t);
        glEnd();
    }

    // Reset transparency settings
    if (lightState == 2 && glIsEnabled(GL_LIGHTING))
    {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);
    }

    glPushMatrix();

    float bulbLocalY = shadeBottomY + 0.4f; // Put bulb inside the shade
    glTranslatef(0.0f, bulbLocalY, 0.0f);

    glColor3f(1.0f, 1.0f, 0.8f);

    // Make the bulb actually glow when lightstate == 2
    if (glIsEnabled(GL_LIGHTING))
    {
        glColor3f(1.0f, 1.0f, 0.8f);
        if (lightState == 2)
        {
            float glow[] = {0.9f, 0.8f, 0.5f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, glow); // Emit light
        }
        else
        {
            float noGlow[] = {0, 0, 0, 1};
            glMaterialfv(GL_FRONT, GL_EMISSION, noGlow); // Stop emitting
        }
    }

    drawSphere(0.25f, 20, 20);

    // Stop emission so other objects don't glow
    if (glIsEnabled(GL_LIGHTING))
    {
        float noGlow[] = {0, 0, 0, 1};
        glMaterialfv(GL_FRONT, GL_EMISSION, noGlow);
    }

    glPopMatrix();

    // If the lamp is ON, tell OpenGL to calculate light rays originating here
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

// Draws the banquet chair
void drawBanquetChair(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Dimensions
    const float seatY = 1.0f;
    const float seatT = 0.25f;
    const float seatW = 1.0f;
    const float seatD = 1.0f;
    const float legH = 1.0f;
    const float legW = 0.07f;
    const float backH = 1.3f;
    const float curveD = 0.25f;

    const float halfT = seatT * 0.5f;
    const float backBaseY = seatY - halfT;
    const float baseZ = -seatD * 0.5f;

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
    }

    // Seat Cushion
    if (glIsEnabled(GL_LIGHTING))
    {
        glBindTexture(GL_TEXTURE_2D, chairCushionTex);
        glColor3f(1, 1, 1);
    }

    glBegin(GL_QUADS);
    float yTop = seatY + halfT;
    float yBot = seatY - halfT;

    // Draw cube faces for seat (Top, Bottom, Left, Right, Front, Back)
    // Top
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yTop, -seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yTop, -seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yTop, seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yTop, seatD / 2);

    // Bottom
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yBot, seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yBot, seatD / 2);

    // Sides
    // Left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(-seatW / 2, yTop, -seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(-seatW / 2, yTop, seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yBot, seatD / 2);

    // Right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yTop, -seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yTop, seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(seatW / 2, yBot, seatD / 2);

    // Front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yBot, seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yBot, seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yTop, seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yTop, seatD / 2);

    // Back
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yBot, -seatD / 2);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yTop, -seatD / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yTop, -seatD / 2);
    glEnd();

    // Legs
    if (glIsEnabled(GL_LIGHTING))
    {
        glBindTexture(GL_TEXTURE_2D, chairLegTex);
    }

    float legOffsetX = seatW / 2 - 0.1f;
    float legOffsetZ = seatD / 2 - 0.1f;

    // Use loop to place 4 legs at corners
    for (int legSignX = -1; legSignX <= 1; legSignX += 2)
        for (int legSignZ = -1; legSignZ <= 1; legSignZ += 2)
        {
            float legCenterX = legSignX * legOffsetX;
            float legCenterZ = legSignZ * legOffsetZ;
            float legBaseY = yBot - legH;

            glBegin(GL_QUADS);
            // Front face of leg
            glNormal3f(0, 0, 1);
            glTexCoord2f(0, 0);
            glVertex3f(legCenterX - legW / 2, legBaseY, legCenterZ - legW / 2);
            glTexCoord2f(1, 0);
            glVertex3f(legCenterX + legW / 2, legBaseY, legCenterZ - legW / 2);
            glTexCoord2f(1, 1);
            glVertex3f(legCenterX + legW / 2, yBot, legCenterZ - legW / 2);
            glTexCoord2f(0, 1);
            glVertex3f(legCenterX - legW / 2, yBot, legCenterZ - legW / 2);
            // Back
            glNormal3f(0, 0, -1);
            glTexCoord2f(0, 0);
            glVertex3f(legCenterX - legW / 2, legBaseY, legCenterZ + legW / 2);
            glTexCoord2f(1, 0);
            glVertex3f(legCenterX + legW / 2, legBaseY, legCenterZ + legW / 2);
            glTexCoord2f(1, 1);
            glVertex3f(legCenterX + legW / 2, yBot, legCenterZ + legW / 2);
            glTexCoord2f(0, 1);
            glVertex3f(legCenterX - legW / 2, yBot, legCenterZ + legW / 2);
            // Left
            glNormal3f(-1, 0, 0);
            glTexCoord2f(0, 0);
            glVertex3f(legCenterX - legW / 2, legBaseY, legCenterZ - legW / 2);
            glTexCoord2f(1, 0);
            glVertex3f(legCenterX - legW / 2, legBaseY, legCenterZ + legW / 2);
            glTexCoord2f(1, 1);
            glVertex3f(legCenterX - legW / 2, yBot, legCenterZ + legW / 2);
            glTexCoord2f(0, 1);
            glVertex3f(legCenterX - legW / 2, yBot, legCenterZ - legW / 2);
            // Right
            glNormal3f(1, 0, 0);
            glTexCoord2f(0, 0);
            glVertex3f(legCenterX + legW / 2, legBaseY, legCenterZ - legW / 2);
            glTexCoord2f(1, 0);
            glVertex3f(legCenterX + legW / 2, legBaseY, legCenterZ + legW / 2);
            glTexCoord2f(1, 1);
            glVertex3f(legCenterX + legW / 2, yBot, legCenterZ + legW / 2);
            glTexCoord2f(0, 1);
            glVertex3f(legCenterX + legW / 2, yBot, legCenterZ - legW / 2);
            glEnd();
        }

    // Curved Backrest
    if (glIsEnabled(GL_LIGHTING))
    {
        glBindTexture(GL_TEXTURE_2D, chairCushionTex);
    }

    int backrestSegments = 20;
    // Sweep vertically to build the backrest
    for (int segmentIndex = 0; segmentIndex < backrestSegments; segmentIndex++)
    {
        float t1 = (float)segmentIndex / backrestSegments;
        float t2 = (float)(segmentIndex + 1) / backrestSegments;

        float y1 = backBaseY + t1 * backH;
        float y2 = backBaseY + t2 * backH;

        // Quadratic curve formula for Z depth
        float z1 = baseZ - curveD * (t1 * t1);
        float z2 = baseZ - curveD * (t2 * t2);

        // Calculate normal vector based on slope of curve
        float dy = y2 - y1;
        float dz = z2 - z1;
        float len = sqrtf(dy * dy + dz * dz);
        float ny = dy / len;
        float nz = dz / len;

        glBegin(GL_QUADS);
        // Front face of backrest
        glNormal3f(0, ny, nz);
        glTexCoord2f(0, t1);
        glVertex3f(-seatW / 2, y1, z1 + halfT);
        glTexCoord2f(1, t1);
        glVertex3f(seatW / 2, y1, z1 + halfT);
        glTexCoord2f(1, t2);
        glVertex3f(seatW / 2, y2, z2 + halfT);
        glTexCoord2f(0, t2);
        glVertex3f(-seatW / 2, y2, z2 + halfT);

        // Back face
        glNormal3f(0, ny, -nz);
        glTexCoord2f(0, t1);
        glVertex3f(-seatW / 2, y1, z1 - halfT);
        glTexCoord2f(1, t1);
        glVertex3f(seatW / 2, y1, z1 - halfT);
        glTexCoord2f(1, t2);
        glVertex3f(seatW / 2, y2, z2 - halfT);
        glTexCoord2f(0, t2);
        glVertex3f(-seatW / 2, y2, z2 - halfT);

        // Side edges
        glNormal3f(-1, 0, 0); // Left edge
        glTexCoord2f(0, t1);
        glVertex3f(-seatW / 2, y1, z1 - halfT);
        glTexCoord2f(1, t1);
        glVertex3f(-seatW / 2, y1, z1 + halfT);
        glTexCoord2f(1, t2);
        glVertex3f(-seatW / 2, y2, z2 + halfT);
        glTexCoord2f(0, t2);
        glVertex3f(-seatW / 2, y2, z2 - halfT);

        glNormal3f(1, 0, 0); // Right edge
        glTexCoord2f(0, t1);
        glVertex3f(seatW / 2, y1, z1 - halfT);
        glTexCoord2f(1, t1);
        glVertex3f(seatW / 2, y1, z1 + halfT);
        glTexCoord2f(1, t2);
        glVertex3f(seatW / 2, y2, z2 + halfT);
        glTexCoord2f(0, t2);
        glVertex3f(seatW / 2, y2, z2 - halfT);
        glEnd();
    }

    // Cap the top of the backrest
    float yCap = backBaseY + backH;
    float zCap = baseZ - curveD;

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-seatW / 2, yCap, zCap - halfT);
    glTexCoord2f(1, 0);
    glVertex3f(seatW / 2, yCap, zCap - halfT);
    glTexCoord2f(1, 1);
    glVertex3f(seatW / 2, yCap, zCap + halfT);
    glTexCoord2f(0, 1);
    glVertex3f(-seatW / 2, yCap, zCap + halfT);
    glEnd();

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

// Draws a Snowflake shaped cocktail table
void drawCocktailTable2(float x, float z)
{
    // Dimensions
    const int N = 8; // Number of points on snowflake
    const int ARC_STEPS = 12;
    const float R = 1.4f;
    const float CURVE_IN = 0.22f;
    const float THICK = 0.12f;
    const float HEIGHT = 4.0f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2Tex);

    // Draw table top surface
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float wedgeAngleDeg = 360.0f / N;
        float a0 = w * wedgeAngleDeg;
        float a1 = (w + 1) * wedgeAngleDeg;

        float prevX = 0.0f, prevZ = 0.0f;

        // Curve the edge of each wedge
        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            // Pinch the radius in the middle of the wedge
            float mid = fabsf(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1.0f - mid);
            float r = R - curve;

            float xArc = r * Cos(a);
            float zArc = r * Sin(a);

            if (i > 0)
            {
                glNormal3f(0, 1, 0);
                glTexCoord2f(0.5f, 0.5f);
                glVertex3f(0, HEIGHT + THICK, 0);

                // Map texture based on angle
                glTexCoord2f((Cos(a0) + 1) * 0.5f, (Sin(a0) + 1) * 0.5f);
                glVertex3f(prevX, HEIGHT + THICK, prevZ);

                glTexCoord2f((Cos(a) + 1) * 0.5f, (Sin(a) + 1) * 0.5f);
                glVertex3f(xArc, HEIGHT + THICK, zArc);
            }

            prevX = xArc;
            prevZ = zArc;
        }
    }
    glEnd();

    // Connect top edge to bottom edge
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N * ARC_STEPS; i++)
    {
        float a = (float)i / (N * ARC_STEPS) * 360.0f;
        float wedgeAngleDeg = 360.0f / N;
        float wedgePos = fmodf(a, wedgeAngleDeg) / wedgeAngleDeg;

        float mid = fabsf(wedgePos - 0.5f) * 2.0f;
        float curve = CURVE_IN * (1.0f - mid);
        float r = R - curve;

        float xSide = r * Cos(a);
        float zSide = r * Sin(a);

        glNormal3f(Cos(a), 0, Sin(a));

        float u = (float)i / (N * ARC_STEPS);
        glTexCoord2f(u, 1);
        glVertex3f(xSide, HEIGHT, zSide);
        glTexCoord2f(u, 0);
        glVertex3f(xSide, HEIGHT + THICK, zSide);
    }
    glEnd();

    // Same as top, but facing down
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float wedgeAngleDeg = 360.0f / N;
        float a0 = w * wedgeAngleDeg;
        float a1 = (w + 1) * wedgeAngleDeg;

        float prevX = 0.0f, prevZ = 0.0f;

        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            float mid = fabsf(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1.0f - mid);
            float r = R - curve;

            float xArc = r * Cos(a);
            float zArc = r * Sin(a);

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

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2LegTex);

    // 3 Angled Legs
    for (int i = 0; i < 3; i++)
    {
        float angDeg = i * 360.0f / 3.0f;
        float lx = 0.8f * Cos(angDeg);
        float lz = 0.8f * Sin(angDeg);

        glPushMatrix();
        glTranslatef(lx, HEIGHT, lz);
        // Tilt the legs outward
        glRotatef(-10, Sin(angDeg), 0, -Cos(angDeg));

        // Draw a tapered cylinder
        glBegin(GL_QUAD_STRIP);
        float LEG_BOTTOM_R = 0.04f;
        float LEG_TOP_R = 0.10f;
        float LEG_H = HEIGHT;

        for (int s = 0; s <= 24; s++)
        {
            float t = 360.0f * s / 24.0f;
            float nx = Cos(t);
            float nz = Sin(t);
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

// Draws a Hourglass shaped cocktail table
void drawCocktailTable3(float x, float z)
{
    int slices = 40;
    float bottomHeight = 2.3f;
    float topHeight = 1.7f;
    float R = 1.35f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3LegTex);

    // Bottom frustum
    drawFrustum(0.85f, 0.45f, bottomHeight, slices);
    glTranslatef(0, bottomHeight, 0);
    glDisable(GL_TEXTURE_2D);

    // Top frustum
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);
    drawFrustum(0.45f, 1.35f, topHeight, slices);
    glDisable(GL_TEXTURE_2D);

    // Top disk
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topHeight, 0);

    for (int i = 0; i <= slices; i++)
    {
        float angDeg = 360.0f * i / slices;
        float cx = Cos(angDeg);
        float cz = Sin(angDeg);

        glTexCoord2f((cx + 1) * 0.5f, (cz + 1) * 0.5f);
        glVertex3f(R * cx, topHeight, R * cz);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Draws a Wavy Meeting Table
void drawMeetingTable(float x, float z)
{
    float length = 8.0f;
    float width = 2.6f;
    float tabletopY = 1.7f;
    float thickness = 0.15f;
    float legHeight = 1.6f;
    float legSize = 0.25f;

    glPushMatrix();

    glTranslatef(x, legHeight * 0.5f, z);
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, meetingTableLegTex);
        glColor3f(1, 1, 1);
    }

    float halfL = length * 0.5f;
    float halfW = width * 0.5f;
    float lx = halfL * 0.55f;
    float lz = halfW * 0.45f;

    // Draw 4 legs
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

    // Table top
    glPushMatrix();
    glTranslatef(x, tabletopY, z);

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, meetingTableTex);
        glColor3f(1, 1, 1);
    }

    // Generate Wavy Edge Geometry
    static float px[200], pz[200];
    int N = 140;

    for (int i = 0; i < N; i++)
    {
        float t = (float)i / (N - 1);
        float angleDeg = t * 360.0f;
        float nx = Cos(angleDeg);
        float ny = Sin(angleDeg);

        // Deform a basic oval
        float rx = halfL * (0.7f + 0.3f * fabsf(ny));
        float ry = halfW;

        // Apply bulge and pinch effects
        float bulge = 1.0f + 0.20f * expf(-powf((fabsf(nx) - 0.3f) * 5.0f, 2.0f));
        float pinch = 1.0f - 0.08f * expf(-powf(nx * 4.0f, 2.0f));

        float finalWidth = ry * bulge * pinch;

        px[i] = rx * nx;
        pz[i] = finalWidth * ny;
    }

    // Draw Top Surface
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

    // Draw Bottom Surface
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

    // Draw Side Edges (Connecting top to bottom loop)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N; i++)
    {
        int k = i % N;
        float sx = px[k];
        float sz = pz[k];

        // Calculate normal vector pointing out from center
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

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

// Draws a rope for ceiling shapes
static void drawRope(float length)
{
    glColor3f(1, 1, 1);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, threadTex);

    glTranslatef(0, -length / 2.0f, 0);
    glScalef(0.05f, length, 0.05f);

    drawCuboid(1, 1, 1);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Draws a Star Shape
void drawStarShape(void)
{
    const float outerR = 1.0f;
    const float innerR = 0.55f;
    const float depth = 0.35f;
    const int numPoints = 5;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, starTex);
    glColor3f(1, 1, 1);

    // Front Face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, depth);

    // Create star points by alternating radius length
    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * 180.0f / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * Cos(ang);
        float y = r * Sin(ang);

        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, depth);
    }
    glEnd();

    // Back Face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, -depth);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * 180.0f / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);
        float x = r * Cos(ang);
        float y = r * Sin(ang);
        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, -depth);
    }
    glEnd();

    // Side Walls
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * 180.0f / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);
        float x = r * Cos(ang);
        float y = r * Sin(ang);
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

// Draws a cloud shape
void drawCloudShape(void)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTex);
    glColor3f(1, 1, 1);

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

// Draws a Crescent Moon shape
static void drawMoonShape(void)
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

    // Front Face
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 360.0f;
        float ox = outerR * Cos(a);
        float oy = outerR * Sin(a);
        float ix = innerR * Cos(a) + offset;
        float iy = innerR * Sin(a);

        // Only draw where Outer is bigger than Inner
        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float u = (Cos(a) + 1) * 0.5f;
            float v = (Sin(a) + 1) * 0.5f;
            glTexCoord2f(u, v);
            glVertex3f(ox, oy, depth);
            glTexCoord2f(u, v);
            glVertex3f(ix, iy, depth);
        }
    }
    glEnd();

    // Back Face
    glNormal3f(0, 0, -1);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 360.0f;
        float ox = outerR * Cos(a);
        float oy = outerR * Sin(a);
        float ix = innerR * Cos(a) + offset;
        float iy = innerR * Sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float u = (Cos(a) + 1) * 0.5f;
            float v = (Sin(a) + 1) * 0.5f;
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
        float a = (float)i / steps * 360.0f;
        float ox = outerR * Cos(a);
        float oy = outerR * Sin(a);
        float ix = innerR * Cos(a) + offset;
        float iy = innerR * Sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            float len = sqrtf(ox * ox + oy * oy);
            glNormal3f(ox / len, oy / len, 0);
            float u = (float)i / steps;
            glTexCoord2f(u, 0);
            glVertex3f(ox, oy, depth);
            glTexCoord2f(u, 1);
            glVertex3f(ox, oy, -depth);
        }
    }
    glEnd();

    // Inner walls
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float a = (float)i / steps * 360.0f;
        float ix = innerR * Cos(a) + offset;
        float iy = innerR * Sin(a);
        float ox = outerR * Cos(a);
        float oy = outerR * Sin(a);

        if ((ox * ox + oy * oy) > (ix * ix + iy * iy))
        {
            // Normal points inward
            float len = sqrtf((ix - offset) * (ix - offset) + iy * iy);
            float nx = -(ix - offset) / len;
            float ny = -iy / len;
            glNormal3f(nx, ny, 0);

            float u = (float)i / steps;
            glTexCoord2f(u, 0);
            glVertex3f(ix, iy, -depth);
            glTexCoord2f(u, 1);
            glVertex3f(ix, iy, depth);
        }
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Draws all ceiling shapes tied using a rope from the ceiling
void drawCeilingShape(float x, float y, float z, int shapeType)
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
    }

    glPopMatrix();
    glPopMatrix();
}

void drawCeilingShapes(void)
{
    // Row 1
    drawCeilingShape(-10, 14.5f, -10, 0); // Star
    drawCeilingShape(0, 14.5f, -10, 1);   // Cloud
    drawCeilingShape(10, 14.5f, -10, 2);  // Moon

    // Row 2
    drawCeilingShape(-10, 14.5f, 0, 1);
    drawCeilingShape(0, 14.5f, 0, 2);
    drawCeilingShape(10, 14.5f, 0, 0);

    // Row 3
    drawCeilingShape(-10, 14.5f, 10, 2);
    drawCeilingShape(0, 14.5f, 10, 0);
    drawCeilingShape(10, 14.5f, 10, 1);
}

// Bar chair dimensions
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

// Draws the soft seat cushion
void drawSeat(void)
{
    float width = 1.0f;
    float halfW = width * 0.5f;
    float rectDepth = 0.55f;
    float radius = halfW;
    int steps = 48;
    float h = CUSHION_THICK;

    glPushMatrix();
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairCushionTex);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Position the seat
    float bottomY = SEAT_HEIGHT - h;
    glTranslatef(0.0f, bottomY + h * 0.5f, 0.0f);

    // Top Face
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, h / 2, rectDepth);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, h / 2, rectDepth);
    // Fan for curved front
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 180.0f * t;
        float x = radius * Cos(a);
        float z = -radius * Sin(a);
        glTexCoord2f(t, 0);
        glVertex3f(x, h / 2, z);
    }
    glEnd();

    // Bottom Face
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, -h / 2, rectDepth);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 180.0f * t;
        float x = radius * Cos(a);
        float z = -radius * Sin(a);
        glTexCoord2f(t, 0);
        glVertex3f(x, -h / 2, z);
    }
    glEnd();

    // Flat Back Wall
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

    // Curved Front Wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 180.0f * t;
        float x = radius * Cos(a);
        float z = -radius * Sin(a);
        glTexCoord2f(t, 0);
        glVertex3f(x, -h / 2, z);
        glTexCoord2f(t, 1);
        glVertex3f(x, +h / 2, z);
    }
    glEnd();

    // Side Walls
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 0);
    glVertex3f(-halfW, -h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(-halfW, +h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-halfW, +h / 2, rectDepth);

    // Right walls
    glTexCoord2f(0, 0);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glTexCoord2f(1, 0);
    glVertex3f(+halfW, -h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(+halfW, +h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glEnd();

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

// Draws the wooden base under the seat
void drawSeatBase(void)
{
    glPushMatrix();
    if (glIsEnabled(GL_LIGHTING))
    {
        glColor3f(0.55f, 0.38f, 0.22f);
    }

    // Dimensions
    const float width = 1.12f;
    const float halfW = width * 0.5f;
    const float rectDepth = 0.62f;
    const float radius = halfW;
    const int steps = 48;
    const float h = BASE_THICK;

    // Position base under cushion
    float bottomY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;
    glTranslatef(0.0f, bottomY + h * 0.5f, 0.0f);

    // Top face
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-halfW, h / 2, rectDepth);
    glVertex3f(+halfW, h / 2, rectDepth);
    for (int i = 0; i <= steps; i++)
    {
        float a = 180.0f * i / steps;
        glVertex3f(radius * Cos(a), h / 2, -radius * Sin(a));
    }
    glEnd();
    // Bottom face
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, -h / 2, rectDepth);
    for (int i = 0; i <= steps; i++)
    {
        float a = 180.0f * i / steps;
        glVertex3f(radius * Cos(a), -h / 2, -radius * Sin(a));
    }
    glEnd();
    // Front wall
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glEnd();
    // Curve
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= steps; i++)
    {
        float a = 180.0f * i / steps;
        float x = radius * Cos(a);
        float z = -radius * Sin(a);
        glVertex3f(x, -h / 2, z);
        glVertex3f(x, +h / 2, z);
    }
    glEnd();
    // Sides
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, rectDepth);
    glVertex3f(-halfW, +h / 2, 0);
    glVertex3f(-halfW, -h / 2, 0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(+halfW, -h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, rectDepth);
    glVertex3f(+halfW, +h / 2, 0);
    glVertex3f(+halfW, -h / 2, 0);
    glEnd();

    glPopMatrix();
}

// Single Chair Leg
void drawLegN(float x, float z)
{
    glPushMatrix();
    if (glIsEnabled(GL_LIGHTING))
        glColor3f(0.45f, 0.30f, 0.18f);

    float legStartY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;
    glTranslatef(x, legStartY - LEG_HEIGHT * 0.5f, z);
    glScalef(0.12f, LEG_HEIGHT, 0.12f);

    drawCuboid(1, 1, 1);
    glPopMatrix();
}

// Draws a curved backrest for the bar chair
void drawCurvedBackrest(void)
{
    float rodBottom = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    float rodTop = rodBottom + (0.75f * CHAIR_SCALE);

    glPushMatrix();
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairBackTex);
        glColor3f(1, 1, 1);
    }

    glTranslatef(0.0f, rodTop, -0.1f);
    glRotatef(-10, 1, 0, 0);
    glRotatef(180, 0, 1, 0);

    const float radius = 0.65f;
    const float height = 0.35f;
    const float thickness = 0.30f;
    const int steps = 160;

    // Draw thin vertical slices
    for (int j = 0; j < steps; j++)
    {
        // Calculate angles for slice
        float a1 = (float)(j - steps / 2);
        float a2 = (float)(j - steps / 2 + 1);
        float x1 = radius * Sin(a1);
        float z1 = radius * Cos(a1);
        float x2 = radius * Sin(a2);
        float z2 = radius * Cos(a2);
        float t0 = (float)j / steps;
        float t1 = (float)(j + 1) / steps;

        glBegin(GL_QUADS);

        // Front
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1);

        // Back
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1 - thickness);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2 - thickness);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1 - thickness);

        // Top
        glTexCoord2f(t0, 1);
        glVertex3f(x1, height, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, height, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, height, z1 - thickness);

        // Bottom
        glTexCoord2f(t0, 1);
        glVertex3f(x1, 0, z1);
        glTexCoord2f(t1, 1);
        glVertex3f(x2, 0, z2);
        glTexCoord2f(t1, 0);
        glVertex3f(x2, 0, z2 - thickness);
        glTexCoord2f(t0, 0);
        glVertex3f(x1, 0, z1 - thickness);

        // Caps for ends
        if (j == 0) // Left Cap
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
        if (j == steps - 1) // Right Cap
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

    if (glIsEnabled(GL_LIGHTING))
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Vertical rod for backrest
void drawBackrestRod(float x, float z)
{
    glPushMatrix();
    if (glIsEnabled(GL_LIGHTING))
        glColor3f(0.45f, 0.30f, 0.18f);

    glTranslatef(x, ROD_BOTTOM + ROD_HEIGHT * 0.5f, z);
    glScalef(0.08f, ROD_HEIGHT, 0.08f);

    drawCuboid(1, 1, 1);
    glPopMatrix();
}

// Full Bar Chair Assembly
void drawBarChair(void)
{
    BASE_THICK = 0.15f * CHAIR_SCALE;
    CUSHION_THICK = 0.18f * CHAIR_SCALE;
    LEG_HEIGHT = 1.2f * CHAIR_SCALE;
    ROD_HEIGHT = 0.75f * CHAIR_SCALE;
    ROD_BOTTOM = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    ROD_TOP = ROD_BOTTOM + ROD_HEIGHT;
    SEAT_HEIGHT = LEG_HEIGHT + BASE_THICK + CUSHION_THICK;

    glPushMatrix();

    // Seat base
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    }
    drawSeatBase();
    if (glIsEnabled(GL_LIGHTING))
        glDisable(GL_TEXTURE_2D);

    // Soft Cushion
    drawSeat();

    // Legs
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    }
    drawLegN(+LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(+LEG_OFFSET_X, -LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, -LEG_OFFSET_Z);
    if (glIsEnabled(GL_LIGHTING))
        glDisable(GL_TEXTURE_2D);

    // Backrest Supports
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    }
    drawBackrestRod(0.0f, -0.50f);
    drawBackrestRod(0.20f, -0.50f);
    drawBackrestRod(-0.20f, -0.50f);
    if (glIsEnabled(GL_LIGHTING))
        glDisable(GL_TEXTURE_2D);

    // Curved backrest
    drawCurvedBackrest();

    glPopMatrix();
}

// Wrapper for Bar Chair
void drawBarChairObj(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(1.0f, 1.2f, 1.0f);
    drawBarChair();
    glPopMatrix();
}

// Draws a log for the fireplace
static void drawLog(float x, float y, float z, float rotY)
{
    float radius = 0.12f;
    float length = 1.4f;

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(90, 0, 0, 1);

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
        glColor3f(0.7f, 0.5f, 0.4f);
    }

    // Center the cylinder
    glTranslatef(0, -length * 0.5f, 0);
    drawCylinder(radius, length, 16);

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1);
    }
    glPopMatrix();
}

// Draws the fireplace
void drawFireplace(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(-90, 0, 1, 0);

    // Apply Brick Texture
    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fireplaceTex);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Base Slab
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    drawCuboid(2.4f, 0.3f, 1.2f);
    glPopMatrix();

    // Pillars
    float pillarH = 2.0f;
    float pillarW = 0.5f;
    float pillarD = 0.8f;
    float pillarX = 0.8f;

    // Left Pillar
    glPushMatrix();
    glTranslatef(-pillarX, 0.3f + pillarH * 0.5f, 0.0f);
    drawCuboid(pillarW, pillarH, pillarD);
    glPopMatrix();

    // Right Pillar
    glPushMatrix();
    glTranslatef(pillarX, 0.3f + pillarH * 0.5f, 0.0f);
    drawCuboid(pillarW, pillarH, pillarD);
    glPopMatrix();

    // Top
    glPushMatrix();
    glTranslatef(0.0f, 0.3f + pillarH + 0.2f, 0.1f);
    drawCuboid(2.6f, 0.4f, 1.4f);
    glPopMatrix();

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Back Wall
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f + pillarH * 0.5f, -0.2f);
    drawCuboid(1.2f, pillarH, 0.1f);
    glPopMatrix();

    // Log 1 (Left bottom)
    drawLog(-0.25f, 0.42f, 0.1f, 10.0f);
    // Log 2 (Right bottom)
    drawLog(0.25f, 0.42f, 0.0f, -5.0f);
    // Log 3 (Top stacked on the other two)
    drawLog(0.0f, 0.60f, 0.05f, 2.0f);

    if (glIsEnabled(GL_LIGHTING))
    {
        glColor3f(1, 1, 1);
    }
    glPopMatrix();
}

// Simple quad for the fire shader to paint on
void drawFirePlane(void)
{
    // Dimensions
    float w = 0.6f;
    float h = 1.0f;
    float yBase = 0.5f;

    glNormal3f(0, 0, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-w, yBase, 0);
    glTexCoord2f(1, 0);
    glVertex3f(w, yBase, 0);
    glTexCoord2f(1, 1);
    glVertex3f(w, yBase + h * 2.0f, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-w, yBase + h * 2.0f, 0);
    glEnd();
}

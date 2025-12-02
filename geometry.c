#include "CSCIx229.h"

// Draw a cylinder using textured quads for the side and triangle fans for caps
void drawCylinder(float radius, float height, int slices)
{
    const float cylinderHeight = height;

    // Side wall
    glBegin(GL_QUAD_STRIP);
    // Sample around the circumference to build the curved surface
    for (int sliceIndex = 0; sliceIndex <= slices; sliceIndex++)
    {
        float angleRadians = 2.0f * M_PI * sliceIndex / slices;
        float unitX = cos(angleRadians);
        float unitZ = sin(angleRadians);

        glNormal3f(unitX, 0, unitZ);

        glTexCoord2f((float)sliceIndex / slices, 0.0f);
        glVertex3f(radius * unitX, 0.0f, radius * unitZ);

        glTexCoord2f((float)sliceIndex / slices, 1.0f);
        glVertex3f(radius * unitX, cylinderHeight, radius * unitZ);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, cylinderHeight, 0.0f);

    // Sweep a fan across the top disk
    for (int sliceIndex = 0; sliceIndex <= slices; sliceIndex++)
    {
        float angleRadians = 2.0f * M_PI * sliceIndex / slices;
        float capX = cos(angleRadians) * radius;
        float capZ = sin(angleRadians) * radius;

        glTexCoord2f(0.5f + 0.5f * cos(angleRadians), 0.5f + 0.5f * sin(angleRadians));
        glVertex3f(capX, cylinderHeight, capZ);
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    // Sweep a fan across the bottom disk
    for (int sliceIndex = 0; sliceIndex <= slices; sliceIndex++)
    {
        float angleRadians = 2.0f * M_PI * sliceIndex / slices;
        float capX = cos(angleRadians) * radius;
        float capZ = sin(angleRadians) * radius;

        glTexCoord2f(0.5f + 0.5f * cos(angleRadians), 0.5f + 0.5f * sin(angleRadians));
        glVertex3f(capX, 0.0f, capZ);
    }
    glEnd();
}

// Draw a sphere using triangle strips per latitude band
void drawSphere(float radius, int slices, int stacks)
{
    for (int stackIndex = 0; stackIndex < stacks; stackIndex++)
    {
        float latitudeStart = M_PI * (-0.5f + (float)stackIndex / stacks);
        float latitudeEnd = M_PI * (-0.5f + (float)(stackIndex + 1) / stacks);

        float sinLatStart = sinf(latitudeStart);
        float cosLatStart = cosf(latitudeStart);
        float sinLatEnd = sinf(latitudeEnd);
        float cosLatEnd = cosf(latitudeEnd);

        glBegin(GL_TRIANGLE_STRIP);

        // Sweep longitudinal slices for this latitude band
        for (int sliceIndex = 0; sliceIndex <= slices; sliceIndex++)
        {
            float longitude = 2.0f * M_PI * (float)sliceIndex / slices;
            float cosLng = cosf(longitude);
            float sinLng = sinf(longitude);

            // first vertex
            glNormal3f(cosLng * cosLatStart, sinLatStart, sinLng * cosLatStart);
            glTexCoord2f((float)sliceIndex / slices, (float)stackIndex / stacks);
            glVertex3f(radius * cosLng * cosLatStart, radius * sinLatStart, radius * sinLng * cosLatStart);

            // second vertex
            glNormal3f(cosLng * cosLatEnd, sinLatEnd, sinLng * cosLatEnd);
            glTexCoord2f((float)sliceIndex / slices, (float)(stackIndex + 1) / stacks);
            glVertex3f(radius * cosLng * cosLatEnd, radius * sinLatEnd, radius * sinLng * cosLatEnd);
        }

        glEnd();
    }
}

// Disk
void drawDisk(float radius, float y, float thickness)
{
    const int segmentCount = 48;
    float topY = y + thickness;

    if (glIsEnabled(GL_LIGHTING))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cocktailTableTex);
        glColor3f(1, 1, 1);
    }

    // top face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topY, 0);

    // Walk around the circle to form the top cap
    for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
    {
        float angleRadians = 2.0f * M_PI * segmentIndex / segmentCount;
        float ringX = radius * cosf(angleRadians);
        float ringZ = radius * sinf(angleRadians);

        float texU = 0.5f + 0.5f * cosf(angleRadians);
        float texV = 0.5f + 0.5f * sinf(angleRadians);

        glTexCoord2f(texU, texV);
        glVertex3f(ringX, topY, ringZ);
    }
    glEnd();

    // bottom face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, y, 0);

    // Walk around the circle to form the bottom cap
    for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
    {
        float angleRadians = 2.0f * M_PI * segmentIndex / segmentCount;
        float ringX = radius * cosf(angleRadians);
        float ringZ = radius * sinf(angleRadians);

        float texU = 0.5f + 0.5f * cosf(angleRadians);
        float texV = 0.5f + 0.5f * sinf(angleRadians);

        glTexCoord2f(texU, texV);
        glVertex3f(ringX, y, ringZ);
    }
    glEnd();

    // side wall
    glBegin(GL_QUAD_STRIP);
    // Extrude the perimeter to give the disk thickness
    for (int segmentIndex = 0; segmentIndex <= segmentCount; segmentIndex++)
    {
        float angleRadians = 2.0f * M_PI * segmentIndex / segmentCount;
        float ringX = radius * cosf(angleRadians);
        float ringZ = radius * sinf(angleRadians);

        glNormal3f(cosf(angleRadians), 0, sinf(angleRadians));
        float texU = (float)segmentIndex / segmentCount;

        glTexCoord2f(texU, 0);
        glVertex3f(ringX, y, ringZ);

        glTexCoord2f(texU, 1);
        glVertex3f(ringX, topY, ringZ);
    }
    glEnd();

    if (glIsEnabled(GL_LIGHTING))
    {
        glDisable(GL_TEXTURE_2D);
    }
}

// Cuboid
// Draw a textured rectangular prism centered at the origin
void drawCuboid(float width, float height, float depth)
{
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;

    glBegin(GL_QUADS);

    // front (+Z)
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(halfWidth, halfHeight, halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(-halfWidth, halfHeight, halfDepth);

    // back (−Z)
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(halfWidth, halfHeight, -halfDepth);

    // left (−X)
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(-halfWidth, halfHeight, halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);

    // right (+X)
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(halfWidth, halfHeight, -halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(halfWidth, halfHeight, halfDepth);

    // top (+Y)
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(-halfWidth, halfHeight, halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(halfWidth, halfHeight, halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(halfWidth, halfHeight, -halfDepth);

    // bottom (−Y)
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 0);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glTexCoord2f(1, 1);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glTexCoord2f(0, 1);
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);

    glEnd();
}

// Frustum
// Draws a truncated cone with textured sides
void drawFrustum(float bottomRadius, float topRadius, float height, int slices)
{
    float angleStep = (2.0f * M_PI) / slices;

    glBegin(GL_TRIANGLES);
    // Iterate around the frustum perimeter to build side triangles
    for (int sliceIndex = 0; sliceIndex < slices; sliceIndex++)
    {
        float angleStart = sliceIndex * angleStep;
        float angleEnd = (sliceIndex + 1) * angleStep;

        float bottomXStart = bottomRadius * cosf(angleStart);
        float bottomZStart = bottomRadius * sinf(angleStart);
        float bottomXEnd = bottomRadius * cosf(angleEnd);
        float bottomZEnd = bottomRadius * sinf(angleEnd);

        float topXStart = topRadius * cosf(angleStart);
        float topZStart = topRadius * sinf(angleStart);
        float topXEnd = topRadius * cosf(angleEnd);
        float topZEnd = topRadius * sinf(angleEnd);

        // Cylindrical texture coordinates
        float texUStart = (float)sliceIndex / slices;
        float texUEnd = (float)(sliceIndex + 1) / slices;

        float normalXStart = bottomXStart + topXStart;
        float normalZStart = bottomZStart + topZStart;
        float normalLengthStart = sqrtf(normalXStart * normalXStart + normalZStart * normalZStart);
        normalXStart /= normalLengthStart;
        normalZStart /= normalLengthStart;

        float normalXEnd = bottomXEnd + topXEnd;
        float normalZEnd = bottomZEnd + topZEnd;
        float normalLengthEnd = sqrtf(normalXEnd * normalXEnd + normalZEnd * normalZEnd);
        normalXEnd /= normalLengthEnd;
        normalZEnd /= normalLengthEnd;

        // First side triangle
        glNormal3f(normalXStart, 0, normalZStart);
        glTexCoord2f(texUStart, 0);
        glVertex3f(bottomXStart, 0, bottomZStart);
        glTexCoord2f(texUStart, 1);
        glVertex3f(topXStart, height, topZStart);
        glNormal3f(normalXEnd, 0, normalZEnd);
        glTexCoord2f(texUEnd, 1);
        glVertex3f(topXEnd, height, topZEnd);

        // Second side triangle
        glNormal3f(normalXStart, 0, normalZStart);
        glTexCoord2f(texUStart, 0);
        glVertex3f(bottomXStart, 0, bottomZStart);
        glNormal3f(normalXEnd, 0, normalZEnd);
        glTexCoord2f(texUEnd, 1);
        glVertex3f(topXEnd, height, topZEnd);
        glTexCoord2f(texUEnd, 0);
        glVertex3f(bottomXEnd, 0, bottomZEnd);
    }
    glEnd();
}

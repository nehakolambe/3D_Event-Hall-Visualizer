#include "CSCIx229.h"

// Draw a cylinder aligned along the Y-axis
void drawCylinder(float radius, float height, int slices)
{
    const float h = height;

    // Side wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float x = cos(a);
        float z = sin(a);

        glNormal3f(x, 0, z);

        glTexCoord2f((float)i / slices, 0.0f);
        glVertex3f(radius * x, 0.0f, radius * z);

        glTexCoord2f((float)i / slices, 1.0f);
        glVertex3f(radius * x, h, radius * z);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, h, 0.0f);

    for (int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float x = cos(a) * radius;
        float z = sin(a) * radius;

        glTexCoord2f(0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a));
        glVertex3f(x, h, z);
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float x = cos(a) * radius;
        float z = sin(a) * radius;

        glTexCoord2f(0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a));
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

// Draw a textured sphere
void drawSphere(float radius, int slices, int stacks)
{
    for (int i = 0; i < stacks; i++)
    {
        float lat0 = M_PI * (-0.5f + (float)i / stacks);
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);

        float z0 = sinf(lat0);
        float zr0 = cosf(lat0);
        float z1 = sinf(lat1);
        float zr1 = cosf(lat1);

        glBegin(GL_TRIANGLE_STRIP);

        for (int j = 0; j <= slices; j++)
        {
            float lng = 2.0f * M_PI * (float)j / slices;
            float x = cosf(lng);
            float y = sinf(lng);

            // first vertex
            glNormal3f(x * zr0, z0, y * zr0);
            glTexCoord2f((float)j / slices, (float)i / stacks);
            glVertex3f(radius * x * zr0, radius * z0, radius * y * zr0);

            // second vertex
            glNormal3f(x * zr1, z1, y * zr1);
            glTexCoord2f((float)j / slices, (float)(i + 1) / stacks);
            glVertex3f(radius * x * zr1, radius * z1, radius * y * zr1);
        }

        glEnd();
    }
}

// Draw a circular disk
void drawDisk(float radius, float y, float thickness)
{
    const int segs = 48;
    float topY = y + thickness;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);
    glColor3f(1, 1, 1);

    // top face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topY, 0);

    for (int i = 0; i <= segs; i++)
    {
        float a = 2.0f * M_PI * i / segs;
        float x = radius * cosf(a);
        float z = radius * sinf(a);

        float u = 0.5f + 0.5f * cosf(a);
        float v = 0.5f + 0.5f * sinf(a);

        glTexCoord2f(u, v);
        glVertex3f(x, topY, z);
    }
    glEnd();

    // bottom face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, y, 0);

    for (int i = 0; i <= segs; i++)
    {
        float a = 2.0f * M_PI * i / segs;
        float x = radius * cosf(a);
        float z = radius * sinf(a);

        float u = 0.5f + 0.5f * cosf(a);
        float v = 0.5f + 0.5f * sinf(a);

        glTexCoord2f(u, v);
        glVertex3f(x, y, z);
    }
    glEnd();

    // side wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++)
    {
        float a = 2.0f * M_PI * i / segs;
        float x = radius * cosf(a);
        float z = radius * sinf(a);

        glNormal3f(cosf(a), 0, sinf(a));
        float u = (float)i / segs;

        glTexCoord2f(u, 0);
        glVertex3f(x, y, z);

        glTexCoord2f(u, 1);
        glVertex3f(x, topY, z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Draw a textured cuboid
void drawCuboid(float width, float height, float depth)
{
    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    glBegin(GL_QUADS);

    // front (+Z)
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-w, -h, d);
    glTexCoord2f(1, 0);
    glVertex3f(w, -h, d);
    glTexCoord2f(1, 1);
    glVertex3f(w, h, d);
    glTexCoord2f(0, 1);
    glVertex3f(-w, h, d);

    // back (−Z)
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(w, -h, -d);
    glTexCoord2f(1, 0);
    glVertex3f(-w, -h, -d);
    glTexCoord2f(1, 1);
    glVertex3f(-w, h, -d);
    glTexCoord2f(0, 1);
    glVertex3f(w, h, -d);

    // left (−X)
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-w, -h, -d);
    glTexCoord2f(1, 0);
    glVertex3f(-w, -h, d);
    glTexCoord2f(1, 1);
    glVertex3f(-w, h, d);
    glTexCoord2f(0, 1);
    glVertex3f(-w, h, -d);

    // right (+X)
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(w, -h, d);
    glTexCoord2f(1, 0);
    glVertex3f(w, -h, -d);
    glTexCoord2f(1, 1);
    glVertex3f(w, h, -d);
    glTexCoord2f(0, 1);
    glVertex3f(w, h, d);

    // top (+Y)
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-w, h, -d);
    glTexCoord2f(1, 0);
    glVertex3f(-w, h, d);
    glTexCoord2f(1, 1);
    glVertex3f(w, h, d);
    glTexCoord2f(0, 1);
    glVertex3f(w, h, -d);

    // bottom (−Y)
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-w, -h, d);
    glTexCoord2f(1, 0);
    glVertex3f(w, -h, d);
    glTexCoord2f(1, 1);
    glVertex3f(w, -h, -d);
    glTexCoord2f(0, 1);
    glVertex3f(-w, -h, -d);

    glEnd();
}

// Draw a frustum using triangle strips
void drawFrustum(float bottomRadius, float topRadius, float height, int slices)
{
    float angleStep = (2.0f * M_PI) / slices;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; i++)
    {
        float a0 = i * angleStep;
        float a1 = (i + 1) * angleStep;

        float x0b = bottomRadius * cosf(a0);
        float z0b = bottomRadius * sinf(a0);
        float x1b = bottomRadius * cosf(a1);
        float z1b = bottomRadius * sinf(a1);

        float x0t = topRadius * cosf(a0);
        float z0t = topRadius * sinf(a0);
        float x1t = topRadius * cosf(a1);
        float z1t = topRadius * sinf(a1);

        // Cylindrical texture coordinates
        float u0 = (float)i / slices;
        float u1 = (float)(i + 1) / slices;

        // Normals for smooth shading
        float nx0 = x0b + x0t;
        float nz0 = z0b + z0t;
        float len0 = sqrtf(nx0 * nx0 + nz0 * nz0);
        nx0 /= len0;
        nz0 /= len0;

        float nx1 = x1b + x1t;
        float nz1 = z1b + z1t;
        float len1 = sqrtf(nx1 * nx1 + nz1 * nz1);
        nx1 /= len1;
        nz1 /= len1;

        // First side triangle
        glNormal3f(nx0, 0, nz0);
        glTexCoord2f(u0, 0);
        glVertex3f(x0b, 0, z0b);
        glTexCoord2f(u0, 1);
        glVertex3f(x0t, height, z0t);
        glNormal3f(nx1, 0, nz1);
        glTexCoord2f(u1, 1);
        glVertex3f(x1t, height, z1t);

        // Second side triangle
        glNormal3f(nx0, 0, nz0);
        glTexCoord2f(u0, 0);
        glVertex3f(x0b, 0, z0b);
        glNormal3f(nx1, 0, nz1);
        glTexCoord2f(u1, 1);
        glVertex3f(x1t, height, z1t);
        glTexCoord2f(u1, 0);
        glVertex3f(x1b, 0, z1b);
    }
    glEnd();
}
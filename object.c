#include "CSCIx229.h"

//
// Draw a rectangular tabletop with slightly rounded corners
//
// static void drawRoundedTop(float length, float width, float y, float thickness, float radius)
// {
//     float hl = length / 2;
//     float hw = width / 2;
//     float topY = y + thickness;

//     glColor3f(0.55, 0.27, 0.07); // wood color

//     // Top surface
//     glBegin(GL_QUADS);
//     glVertex3f(-hl, topY, -hw);
//     glVertex3f( hl, topY, -hw);
//     glVertex3f( hl, topY,  hw);
//     glVertex3f(-hl, topY,  hw);
//     glEnd();

//     // Side faces (tabletop thickness)
//     glColor3f(0.45, 0.23, 0.1);
//     glBegin(GL_QUADS);
//     // front
//     glVertex3f(-hl, y,  hw);
//     glVertex3f( hl, y,  hw);
//     glVertex3f( hl, topY,  hw);
//     glVertex3f(-hl, topY,  hw);
//     // back
//     glVertex3f(-hl, y, -hw);
//     glVertex3f( hl, y, -hw);
//     glVertex3f( hl, topY, -hw);
//     glVertex3f(-hl, topY, -hw);
//     // left
//     glVertex3f(-hl, y, -hw);
//     glVertex3f(-hl, y,  hw);
//     glVertex3f(-hl, topY,  hw);
//     glVertex3f(-hl, topY, -hw);
//     // right
//     glVertex3f( hl, y, -hw);
//     glVertex3f( hl, y,  hw);
//     glVertex3f( hl, topY,  hw);
//     glVertex3f( hl, topY, -hw);
//     glEnd();
// }

void drawCylinder(float radius, float height, int slices)
{
    float halfH = height;

    /* ===== SIDE WALL ===== */
    glBegin(GL_QUAD_STRIP);
    for(int i=0; i<=slices; i++)
    {
        float a = 2*M_PI*i/slices;
        float x = cos(a);
        float z = sin(a);

        glNormal3f(x,0,z);

        glTexCoord2f((float)i/slices,0);
        glVertex3f(radius*x, 0,        radius*z);

        glTexCoord2f((float)i/slices,1);
        glVertex3f(radius*x, height,   radius*z);
    }
    glEnd();

    /* ===== TOP CAP ===== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glTexCoord2f(0.5,0.5);
    glVertex3f(0,height,0);

    for(int i=0;i<=slices;i++){
        float a = 2*M_PI*i/slices;
        float x = radius*cos(a);
        float z = radius*sin(a);

        glTexCoord2f(0.5+0.5*cos(a), 0.5+0.5*sin(a));
        glVertex3f(x,height,z);
    }
    glEnd();

    /* ===== BOTTOM CAP ===== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glTexCoord2f(0.5,0.5);
    glVertex3f(0,0,0);

    for(int i=0;i<=slices;i++){
        float a = 2*M_PI*i/slices;
        float x = radius*cos(a);
        float z = radius*sin(a);

        glTexCoord2f(0.5+0.5*cos(a), 0.5+0.5*sin(a));
        glVertex3f(x,0,z);
    }
    glEnd();
}


//
// Draw one cylindrical leg standing vertically (Y-axis)
//
static void drawLeg(float x, float z, float height, float radius)
{
    int slices = 32;   // same as before

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    glPushMatrix();

    // Position cylinder upright like your old gluCylinder rotated -90° on X
    glTranslatef(x, 0, z);

    glColor3f(1,1,1);  // let texture appear fully

    /* ======================================================
       Draw Cylinder Side (exact same shape as gluCylinder)
       ====================================================== */
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float cx = cos(a);
        float cz = sin(a);

        glNormal3f(cx, 0, cz);

        glTexCoord2f((float)i/slices, 0.0f);
        glVertex3f(radius*cx, 0.0f, radius*cz);

        glTexCoord2f((float)i/slices, 1.0f);
        glVertex3f(radius*cx, height, radius*cz);
    }
    glEnd();

    /* ======================================================
       Top Cap
       ====================================================== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, height, 0);

    for(int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float cx = cos(a) * radius;
        float cz = sin(a) * radius;

        glTexCoord2f(0.5f + 0.5f*cos(a), 0.5f + 0.5f*sin(a));
        glVertex3f(cx, height, cz);
    }
    glEnd();

    /* ======================================================
       Bottom Cap
       ====================================================== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, 0);

    for(int i = 0; i <= slices; i++)
    {
        float a = 2.0f * M_PI * i / slices;
        float cx = cos(a) * radius;
        float cz = sin(a) * radius;

        glTexCoord2f(0.5f + 0.5f*cos(a), 0.5f + 0.5f*sin(a));
        glVertex3f(cx, 0, cz);
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


void drawSphere(float radius, int slices, int stacks)
{
    for (int i = 0; i < stacks; i++)
    {
        float lat0 = M_PI * (-0.5 + (float)i / stacks);
        float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);

        float z0 = sin(lat0), zr0 = cos(lat0);
        float z1 = sin(lat1), zr1 = cos(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float lng = 2 * M_PI * (float)(j) / slices;
            float x = cos(lng);
            float y = sin(lng);

            // ----- vertex 1 -----
            glNormal3f(x * zr0, z0, y * zr0);
            glTexCoord2f((float)j / slices, (float)i / stacks);
            glVertex3f(radius * x * zr0, radius * z0, radius * y * zr0);

            // ----- vertex 2 -----
            glNormal3f(x * zr1, z1, y * zr1);
            glTexCoord2f((float)j / slices, (float)(i + 1) / stacks);
            glVertex3f(radius * x * zr1, radius * z1, radius * y * zr1);
        }
        glEnd();
    }
}


void drawCube()
{
    glBegin(GL_QUADS);

    // +X
    glNormal3f(1,0,0);
    glTexCoord2f(1,0); glVertex3f(0.5, -0.5, -0.5);
    glTexCoord2f(1,1); glVertex3f(0.5,  0.5, -0.5);
    glTexCoord2f(0,1); glVertex3f(0.5,  0.5,  0.5);
    glTexCoord2f(0,0); glVertex3f(0.5, -0.5,  0.5);

    // -X
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-0.5, -0.5, -0.5);
    glTexCoord2f(1,0); glVertex3f(-0.5, -0.5,  0.5);
    glTexCoord2f(1,1); glVertex3f(-0.5,  0.5,  0.5);
    glTexCoord2f(0,1); glVertex3f(-0.5,  0.5, -0.5);

    // +Y (top)
    glNormal3f(0,1,0);
    glTexCoord2f(0,1); glVertex3f(-0.5, 0.5, -0.5);
    glTexCoord2f(0,0); glVertex3f(-0.5, 0.5,  0.5);
    glTexCoord2f(1,0); glVertex3f( 0.5, 0.5,  0.5);
    glTexCoord2f(1,1); glVertex3f( 0.5, 0.5, -0.5);

    // -Y (bottom)
    glNormal3f(0,-1,0);
    glTexCoord2f(0,1); glVertex3f(-0.5, -0.5, -0.5);
    glTexCoord2f(1,1); glVertex3f( 0.5, -0.5, -0.5);
    glTexCoord2f(1,0); glVertex3f( 0.5, -0.5,  0.5);
    glTexCoord2f(0,0); glVertex3f(-0.5, -0.5,  0.5);

    // +Z
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-0.5, -0.5, 0.5);
    glTexCoord2f(1,0); glVertex3f( 0.5, -0.5, 0.5);
    glTexCoord2f(1,1); glVertex3f( 0.5,  0.5, 0.5);
    glTexCoord2f(0,1); glVertex3f(-0.5,  0.5, 0.5);

    // -Z
    glNormal3f(0,0,-1);
    glTexCoord2f(1,0); glVertex3f( 0.5, -0.5,-0.5);
    glTexCoord2f(1,1); glVertex3f( 0.5,  0.5,-0.5);
    glTexCoord2f(0,1); glVertex3f(-0.5,  0.5,-0.5);
    glTexCoord2f(0,0); glVertex3f(-0.5, -0.5,-0.5);

    glEnd();
}


//
// Draw a circular disk (for table top or base)
//
static void drawDisk(float radius, float y, float thickness)
{
    int segs = 48;
    float topY = y + thickness;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);
    glColor3f(1,1,1);   // allow texture colors to show fully

    /* ===== TOP FACE ===== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topY, 0);

    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        glTexCoord2f(u, v);
        glVertex3f(x, topY, z);
    }
    glEnd();


    /* ===== BOTTOM FACE ===== */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, y, 0);

    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        glTexCoord2f(u, v);
        glVertex3f(x, y, z);
    }
    glEnd();


    /* ===== SIDE WALL ===== */
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        float nx = cos(angle);
        float nz = sin(angle);

        glNormal3f(nx, 0, nz);

        float u = (float)i / segs;

        glTexCoord2f(u, 0);
        glVertex3f(x, y, z);

        glTexCoord2f(u, 1);
        glVertex3f(x, topY, z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


static void drawCuboid(float width, float height, float depth)
{
    float w = width  / 2.0f;
    float h = height / 2.0f;
    float d = depth  / 2.0f;

    glBegin(GL_QUADS);

    /* ===== FRONT (+Z) ===== */
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-w,-h, d);
    glTexCoord2f(1,0); glVertex3f( w,-h, d);
    glTexCoord2f(1,1); glVertex3f( w, h, d);
    glTexCoord2f(0,1); glVertex3f(-w, h, d);

    /* ===== BACK (−Z) ===== */
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f( w,-h,-d);
    glTexCoord2f(1,0); glVertex3f(-w,-h,-d);
    glTexCoord2f(1,1); glVertex3f(-w, h,-d);
    glTexCoord2f(0,1); glVertex3f( w, h,-d);

    /* ===== LEFT (−X) ===== */
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-w,-h,-d);
    glTexCoord2f(1,0); glVertex3f(-w,-h, d);
    glTexCoord2f(1,1); glVertex3f(-w, h, d);
    glTexCoord2f(0,1); glVertex3f(-w, h,-d);

    /* ===== RIGHT (+X) ===== */
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f( w,-h, d);
    glTexCoord2f(1,0); glVertex3f( w,-h,-d);
    glTexCoord2f(1,1); glVertex3f( w, h,-d);
    glTexCoord2f(0,1); glVertex3f( w, h, d);

    /* ===== TOP (+Y) ===== */
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-w, h,-d);
    glTexCoord2f(1,0); glVertex3f(-w, h, d);
    glTexCoord2f(1,1); glVertex3f( w, h, d);
    glTexCoord2f(0,1); glVertex3f( w, h,-d);

    /* ===== BOTTOM (−Y) ===== */
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-w,-h, d);
    glTexCoord2f(1,0); glVertex3f( w,-h, d);
    glTexCoord2f(1,1); glVertex3f( w,-h,-d);
    glTexCoord2f(0,1); glVertex3f(-w,-h,-d);

    glEnd();
}



//
// Draw a rectangular table with rounded corners (no GLU/GLUT)
//

void drawTable(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);

    float tabletopY = 2.0f;
    float thickness = 0.15f;
    float legHeight = tabletopY;
    float legSize = 0.25f;
    float length = 4.0f;
    float width  = 2.0f;
    float radius = 0.3f;
    int segs = 20;

    float yBottom = tabletopY;
    float yTop    = tabletopY + thickness;
    float innerX = length/2 - radius;
    float innerZ = width /2 - radius;

    /* =====================================
          ENABLE TEXTURE FOR WHOLE TABLE
    ===================================== */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableTex);
    glColor3f(1,1,1);  // allow texture to show


    /*===============================
       TOP SURFACE (with texture)
    ===============================*/
    glNormal3f(0,1,0);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-innerX, yTop, -innerZ);
        glTexCoord2f(1,0); glVertex3f( innerX, yTop, -innerZ);
        glTexCoord2f(1,1); glVertex3f( innerX, yTop,  innerZ);
        glTexCoord2f(0,1); glVertex3f(-innerX, yTop,  innerZ);
    glEnd();


    /*===============================
        FLAT EDGE PADS (textured)
    ===============================*/
    glBegin(GL_QUADS);

    // front
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-innerX, yTop, innerZ);
    glTexCoord2f(1,0); glVertex3f( innerX, yTop, innerZ);
    glTexCoord2f(1,1); glVertex3f( innerX, yTop, innerZ + radius);
    glTexCoord2f(0,1); glVertex3f(-innerX, yTop, innerZ + radius);

    // back
    glTexCoord2f(0,0); glVertex3f(-innerX, yTop, -innerZ - radius);
    glTexCoord2f(1,0); glVertex3f( innerX, yTop, -innerZ - radius);
    glTexCoord2f(1,1); glVertex3f( innerX, yTop, -innerZ);
    glTexCoord2f(0,1); glVertex3f(-innerX, yTop, -innerZ);

    // left
    glTexCoord2f(0,0); glVertex3f(-innerX - radius, yTop, -innerZ);
    glTexCoord2f(1,0); glVertex3f(-innerX,          yTop, -innerZ);
    glTexCoord2f(1,1); glVertex3f(-innerX,          yTop,  innerZ);
    glTexCoord2f(0,1); glVertex3f(-innerX - radius, yTop,  innerZ);

    // right
    glTexCoord2f(0,0); glVertex3f(innerX,          yTop, -innerZ);
    glTexCoord2f(1,0); glVertex3f(innerX + radius, yTop, -innerZ);
    glTexCoord2f(1,1); glVertex3f(innerX + radius, yTop,  innerZ);
    glTexCoord2f(0,1); glVertex3f(innerX,          yTop,  innerZ);

    glEnd();


    /*===============================
      ROUNDED CORNERS (top texture)
    ===============================*/
    for (int c = 0; c < 4; c++)
    {
        float cx = (c==0||c==3) ? innerX : -innerX;
        float cz = (c<2) ? innerZ : -innerZ;

        float start, end;
        if(c==0){start=0; end=90;}
        else if(c==1){start=90; end=180;}
        else if(c==2){start=180; end=270;}
        else        {start=270; end=360;}

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,1,0);

        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(cx, yTop, cz);

        for(int i=0;i<=segs;i++){
            float a = (start + i*(end-start)/segs) * M_PI/180.0f;
            float xPos = cx + radius*cos(a);
            float zPos = cz + radius*sin(a);

            float u = 0.5f + 0.5f*cos(a);
            float v = 0.5f + 0.5f*sin(a);

            glTexCoord2f(u, v);
            glVertex3f(xPos, yTop, zPos);
        }
        glEnd();
    }


    /*===============================
      CURVED SIDE WALLS (textured)
    ===============================*/
    for (int c = 0; c < 4; c++)
    {
        float cx = (c==0||c==3) ? innerX : -innerX;
        float cz = (c<2) ? innerZ : -innerZ;

        float start, end;
        if(c==0){start=0; end=90;}
        else if(c==1){start=90; end=180;}
        else if(c==2){start=180; end=270;}
        else        {start=270; end=360;}

        glBegin(GL_QUAD_STRIP);
        for(int i=0;i<=segs;i++)
        {
            float a = (start + i*(end-start)/segs) * M_PI/180.0f;

            float xPos = cx + radius*cos(a);
            float zPos = cz + radius*sin(a);
            float nx = cos(a);
            float nz = sin(a);

            glNormal3f(nx,0,nz);

            float u = (float)i/segs;

            glTexCoord2f(u,0); glVertex3f(xPos,yBottom,zPos);
            glTexCoord2f(u,1); glVertex3f(xPos,yTop,   zPos);
        }
        glEnd();
    }


    /*===============================
      FLAT SIDE WALLS (textured)
    ===============================*/
    glBegin(GL_QUADS);

    // front
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-innerX, yBottom, innerZ+radius);
    glTexCoord2f(1,0); glVertex3f( innerX, yBottom, innerZ+radius);
    glTexCoord2f(1,1); glVertex3f( innerX, yTop,    innerZ+radius);
    glTexCoord2f(0,1); glVertex3f(-innerX, yTop,    innerZ+radius);

    // back
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f(-innerX, yBottom, -innerZ-radius);
    glTexCoord2f(1,0); glVertex3f( innerX, yBottom, -innerZ-radius);
    glTexCoord2f(1,1); glVertex3f( innerX, yTop,    -innerZ-radius);
    glTexCoord2f(0,1); glVertex3f(-innerX, yTop,    -innerZ-radius);

    // left
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-innerX-radius, yBottom, -innerZ);
    glTexCoord2f(1,0); glVertex3f(-innerX-radius, yBottom,  innerZ);
    glTexCoord2f(1,1); glVertex3f(-innerX-radius, yTop,     innerZ);
    glTexCoord2f(0,1); glVertex3f(-innerX-radius, yTop,    -innerZ);

    // right
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f(innerX+radius, yBottom, -innerZ);
    glTexCoord2f(1,0); glVertex3f(innerX+radius, yBottom,  innerZ);
    glTexCoord2f(1,1); glVertex3f(innerX+radius, yTop,     innerZ);
    glTexCoord2f(0,1); glVertex3f(innerX+radius, yTop,    -innerZ);

    glEnd();


    /*===============================
      LEGS  (use drawCuboid)
    ===============================*/
    float offsetX = innerX;
    float offsetZ = innerZ;

    float pos[4][2] = {
        { offsetX,  offsetZ},
        {-offsetX,  offsetZ},
        {-offsetX, -offsetZ},
        { offsetX, -offsetZ}
    };

    for(int i=0;i<4;i++){
        glPushMatrix();
        glTranslatef(pos[i][0], legHeight/2, pos[i][1]);
        drawCuboid(legSize, legHeight, legSize);  // supports texture if implemented
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}




//
// Cocktail table (tall, circular, one leg)
//
void drawCocktailTable(float x, float z)
{
    float height = 4.0f;
    float topRadius = 1.5f;
    float topThickness = 0.1f;
    float legRadius = 0.15f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    // Draw leg (textured cylinder)
    drawLeg(0, 0, height, legRadius);

    // Draw circular tabletop (textured disk)
    drawDisk(topRadius, height, topThickness);

    // Draw base disk (also textured)
    drawDisk(0.6f, 0.0f, 0.05f);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

//
// Draw a DOOR with correct normals
//
void drawDoor(float x, float z, float width, float height)
{
    float yBottom = 0.0;
    float yTop    = height;
    float halfW   = width / 2.0;

    float nx = 0.0, ny = 0.0, nz = -1.0;   // front wall

    /* ======================================================
                DOOR FRAME  (uses doorFrameTex)
       ====================================================== */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorFrameTex);

    glBegin(GL_QUADS);

    // ---- Left frame ----
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0); glVertex3f(x - halfW - 0.1, yBottom, z + 0.01);
    glTexCoord2f(1, 0); glVertex3f(x - halfW,       yBottom, z + 0.01);
    glTexCoord2f(1, 1); glVertex3f(x - halfW,       yTop,    z + 0.01);
    glTexCoord2f(0, 1); glVertex3f(x - halfW - 0.1, yTop,    z + 0.01);

    // ---- Right frame ----
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0); glVertex3f(x + halfW,       yBottom, z + 0.01);
    glTexCoord2f(1, 0); glVertex3f(x + halfW + 0.1, yBottom, z + 0.01);
    glTexCoord2f(1, 1); glVertex3f(x + halfW + 0.1, yTop,    z + 0.01);
    glTexCoord2f(0, 1); glVertex3f(x + halfW,       yTop,    z + 0.01);

    // ---- Top frame ----
    glNormal3f(nx, ny, nz);
    glTexCoord2f(0, 0); glVertex3f(x - halfW - 0.1, yTop, z + 0.01);
    glTexCoord2f(1, 0); glVertex3f(x + halfW + 0.1, yTop, z + 0.01);
    glTexCoord2f(1, 1); glVertex3f(x + halfW + 0.1, yTop + 0.1, z + 0.01);
    glTexCoord2f(0, 1); glVertex3f(x - halfW - 0.1, yTop + 0.1, z + 0.01);

    glEnd();


    /* ======================================================
                   DOOR PANEL (flat front)  
                   Uses *same* texture as frame
       ====================================================== */

    glBindTexture(GL_TEXTURE_2D, doorFrameTex);

    glBegin(GL_QUADS);
    glNormal3f(nx, ny, nz);

    glTexCoord2f(0, 0); glVertex3f(x - halfW, yBottom, z);
    glTexCoord2f(1, 0); glVertex3f(x + halfW, yBottom, z);
    glTexCoord2f(1, 1); glVertex3f(x + halfW, yTop,    z);
    glTexCoord2f(0, 1); glVertex3f(x - halfW, yTop,    z);

    glEnd();

    glDisable(GL_TEXTURE_2D);


    /* ======================================================
                  DOOR KNOB (textured sphere)
       ====================================================== */
    glPushMatrix();
    glTranslatef(x + halfW - 0.3, yBottom + height * 0.5, z + 0.05);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, doorKnobTex);

    // enable automatic texture coords for sphere
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    drawSphere(0.12, 20, 20);

    // disable automatic coords
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

//
// Draw a curved LED screen mounted on the wall
//   - curvature controlled by "curveRadius"
//   - width/height define total size
//   - zOffset determines how far from wall (negative = closer to audience)
//
void drawCurvedScreen(float wallX, float wallZ, float width, float height,
                      float yBase, float radiusH, float radiusV, float zOffset)
{
    int segH = 64;  // horizontal segments
    int segV = 32;  // vertical segments

    float halfHeight = height / 2.0f;

    float angleH = width  / radiusH; // horizontal angular span
    float angleV = height / radiusV; // vertical angular span

    glPushMatrix();
    glTranslatef(wallX, yBase + halfHeight, wallZ);

    glColor3f(1, 1, 1);

    glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, screenTex);

glBegin(GL_QUADS);
for (int i = 0; i < segH; i++)
{
    float u1 = (float)i     / segH;
    float u2 = (float)(i+1) / segH;

    float t1 = -angleH/2 + angleH * u1;
    float t2 = -angleH/2 + angleH * u2;

    for (int j = 0; j < segV; j++)
    {
        float v1 = (float)j     / segV;
        float v2 = (float)(j+1) / segV;

        float p1 = -angleV/2 + angleV * v1;
        float p2 = -angleV/2 + angleV * v2;

        // Compute vertex positions
        float x11 = radiusH * sin(t1);
        float z11 = -radiusH * cos(t1) + radiusH + zOffset;
        float y11 = radiusV * sin(p1);

        float x21 = radiusH * sin(t2);
        float z21 = -radiusH * cos(t2) + radiusH + zOffset;
        float y21 = radiusV * sin(p1);

        float x22 = radiusH * sin(t2);
        float z22 = -radiusH * cos(t2) + radiusH + zOffset;
        float y22 = radiusV * sin(p2);

        float x12 = radiusH * sin(t1);
        float z12 = -radiusH * cos(t1) + radiusH + zOffset;
        float y12 = radiusV * sin(p2);

        // compute normals (your existing code)
        float nx_t1 = sin(t1), nz_t1 = cos(t1);
        float nx_t2 = sin(t2), nz_t2 = cos(t2);
        float ny_p1 = sin(p1), ny_p2 = sin(p2);

        // --- draw with UV mapping ---
        glNormal3f(nx_t1, ny_p1, nz_t1);
        glTexCoord2f(u1, v1);
        glVertex3f(x11, y11, z11);

        glNormal3f(nx_t2, ny_p1, nz_t2);
        glTexCoord2f(u2, v1);
        glVertex3f(x21, y21, z21);

        glNormal3f(nx_t2, ny_p2, nz_t2);
        glTexCoord2f(u2, v2);
        glVertex3f(x22, y22, z22);

        glNormal3f(nx_t1, ny_p2, nz_t1);
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
    float shadeTopRadius   = 0.5f;
    float shadeHeight = 0.6f;

    glPushMatrix();
    glTranslatef(xPos, baseHeight, zPos);



    /* ============================================================
         BASE  (textured using lampRodTex)
       ============================================================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lampRodTex);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, 0);

    for (int i = 0; i <= segments; i++)
    {
        float t = 2 * PI * i / segments;
        float u = (cos(t) + 1) * 0.5f;
        float v = (sin(t) + 1) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(baseRadius*cos(t), 0, baseRadius*sin(t));
    }
    glEnd();



    /* ============================================================
         POLE (cylinder, textured using lampRodTex)
       ============================================================ */
    for (int i = 0; i < segments; i++)
    {
        float t1 = 2 * PI * i / segments;
        float t2 = 2 * PI * (i + 1) / segments;

        float x1 = poleRadius * cos(t1);
        float z1 = poleRadius * sin(t1);
        float x2 = poleRadius * cos(t2);
        float z2 = poleRadius * sin(t2);

        float nx1 = cos(t1), nz1 = sin(t1);
        float nx2 = cos(t2), nz2 = sin(t2);

        glBegin(GL_QUADS);

        glNormal3f(nx1,0,nz1);  glTexCoord2f((float)i/segments,0); glVertex3f(x1,0,z1);
        glNormal3f(nx2,0,nz2);  glTexCoord2f((float)(i+1)/segments,0); glVertex3f(x2,0,z2);
        glNormal3f(nx2,0,nz2);  glTexCoord2f((float)(i+1)/segments,1); glVertex3f(x2,poleHeight,z2);
        glNormal3f(nx1,0,nz1);  glTexCoord2f((float)i/segments,1); glVertex3f(x1,poleHeight,z1);

        glEnd();
    }



    /* ============================================================
         SHADE (cone trunk, textured using lampShadeTex)
       ============================================================ */
    glBindTexture(GL_TEXTURE_2D, lampShadeTex);

    float shadeBottomY = poleHeight;
    float shadeTopY    = poleHeight + shadeHeight;

    for (int i = 0; i < segments; i++)
    {
        float t1 = 2 * PI * i / segments;
        float t2 = 2 * PI * (i + 1) / segments;

        float x1b = shadeBottomRadius * cos(t1);
        float z1b = shadeBottomRadius * sin(t1);
        float x2b = shadeBottomRadius * cos(t2);
        float z2b = shadeBottomRadius * sin(t2);

        float x1t = shadeTopRadius * cos(t1);
        float z1t = shadeTopRadius * sin(t1);
        float x2t = shadeTopRadius * cos(t2);
        float z2t = shadeTopRadius * sin(t2);

        float nx1 = cos(t1), nz1 = sin(t1);
        float nx2 = cos(t2), nz2 = sin(t2);

        glBegin(GL_QUADS);

        glNormal3f(nx1,0,nz1);  glTexCoord2f((float)i/segments,0); glVertex3f(x1b,shadeBottomY,z1b);
        glNormal3f(nx2,0,nz2);  glTexCoord2f((float)(i+1)/segments,0); glVertex3f(x2b,shadeBottomY,z2b);
        glNormal3f(nx2,0,nz2);  glTexCoord2f((float)(i+1)/segments,1); glVertex3f(x2t,shadeTopY,z2t);
        glNormal3f(nx1,0,nz1);  glTexCoord2f((float)i/segments,1); glVertex3f(x1t,shadeTopY,z1t);

        glEnd();
    }



    /* ============================================================
         SHADE TOP CAP  (textured, but inverted UV center)
       ============================================================ */
    glBegin(GL_TRIANGLE_FAN);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, shadeTopY, 0);

    for (int i = 0; i <= segments; i++)
    {
        float t = 2 * PI * i / segments;
        float u = (cos(t) + 1) * 0.5f;
        float v = (sin(t) + 1) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(shadeTopRadius*cos(t), shadeTopY, shadeTopRadius*sin(t));
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);



    /* ============================================================
         BULB + LIGHT — UNCHANGED
       ============================================================ */
    if (lightState == 2)
    {
        glPushMatrix();

        float bulbLocalY = shadeBottomY + 0.4f;
        glTranslatef(0, bulbLocalY, 0);

        float bulbEmission[] = {1.0,0.9,0.7,1.0};
        glMaterialfv(GL_FRONT, GL_EMISSION, bulbEmission);

        drawSphere(0.25f, 20, 20);

        float noEmission[] = {0,0,0,1};
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

        glPopMatrix();

        float bulbWorldX = xPos;
        float bulbWorldY = baseHeight + bulbLocalY;
        float bulbWorldZ = zPos;

        float pos1[] = {bulbWorldX, bulbWorldY, bulbWorldZ, 1.0};
        glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    }


    glPopMatrix();
}



void drawBanquetChair(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);

    float seatY = 1.0f;
    float thickness = 0.25f;
    float seatW = 1.0f;
    float seatD = 1.0f;
    float legH = 1.0f;

    float backHeight = 1.3f;
    float curveDepth = 0.25f;
    int segments = 20;

    float halfT = thickness * 0.5f;
    float baseZ = -seatD / 2.0f;
    float backBaseY = seatY - thickness / 2.0f;

    /* ============================================================
                    SEAT — CUSHION TEXTURE
       ============================================================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, chairCushionTex);

    glBegin(GL_QUADS);

    // Top
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, seatY+halfT, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, seatY+halfT, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, seatY+halfT,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, seatY+halfT,  seatD/2);

    // Bottom
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, seatY-halfT,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, seatY-halfT,  seatD/2);

    // Left
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,0); glVertex3f(-seatW/2, seatY+halfT, -seatD/2);
    glTexCoord2f(1,1); glVertex3f(-seatW/2, seatY+halfT,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, seatY-halfT,  seatD/2);

    // Right
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f(seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,0); glVertex3f(seatW/2, seatY+halfT, -seatD/2);
    glTexCoord2f(1,1); glVertex3f(seatW/2, seatY+halfT,  seatD/2);
    glTexCoord2f(0,1); glVertex3f(seatW/2, seatY-halfT,  seatD/2);

    // Front
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, seatY-halfT, seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, seatY-halfT, seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, seatY+halfT, seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, seatY+halfT, seatD/2);

    // Back
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,0); glVertex3f( seatW/2, seatY-halfT, -seatD/2);
    glTexCoord2f(1,1); glVertex3f( seatW/2, seatY+halfT, -seatD/2);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, seatY+halfT, -seatD/2);

    glEnd();



    /* ============================================================
                    LEGS — LEG TEXTURE
       ============================================================ */
    glBindTexture(GL_TEXTURE_2D, chairLegTex);

    float legW = 0.07f;
    float halfW = seatW/2 - 0.1f;
    float halfD = seatD/2 - 0.1f;

    float legTop = seatY - halfT;
    float legBottom = legTop - legH;

    for (int i = -1; i <= 1; i += 2)
    for (int j = -1; j <= 1; j += 2)
    {
        float lx = i * halfW;
        float lz = j * halfD;

        glBegin(GL_QUADS);
        // Front
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(lx-legW/2, legBottom, lz-legW/2);
        glTexCoord2f(1,0); glVertex3f(lx+legW/2, legBottom, lz-legW/2);
        glTexCoord2f(1,1); glVertex3f(lx+legW/2, legTop,    lz-legW/2);
        glTexCoord2f(0,1); glVertex3f(lx-legW/2, legTop,    lz-legW/2);

        // Back
        glNormal3f(0,0,-1);
        glTexCoord2f(0,0); glVertex3f(lx-legW/2, legBottom, lz+legW/2);
        glTexCoord2f(1,0); glVertex3f(lx+legW/2, legBottom, lz+legW/2);
        glTexCoord2f(1,1); glVertex3f(lx+legW/2, legTop,    lz+legW/2);
        glTexCoord2f(0,1); glVertex3f(lx-legW/2, legTop,    lz+legW/2);

        // Left
        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(lx-legW/2, legBottom, lz-legW/2);
        glTexCoord2f(1,0); glVertex3f(lx-legW/2, legBottom, lz+legW/2);
        glTexCoord2f(1,1); glVertex3f(lx-legW/2, legTop,    lz+legW/2);
        glTexCoord2f(0,1); glVertex3f(lx-legW/2, legTop,    lz-legW/2);

        // Right
        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(lx+legW/2, legBottom, lz-legW/2);
        glTexCoord2f(1,0); glVertex3f(lx+legW/2, legBottom, lz+legW/2);
        glTexCoord2f(1,1); glVertex3f(lx+legW/2, legTop,    lz+legW/2);
        glTexCoord2f(0,1); glVertex3f(lx+legW/2, legTop,    lz-legW/2);

        glEnd();
    }



    /* ============================================================
          CURVED BACKREST — CUSHION TEXTURE
       ============================================================ */
    glBindTexture(GL_TEXTURE_2D, chairCushionTex);

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i+1) / segments;

        float y1 = backBaseY + t1 * backHeight;
        float y2 = backBaseY + t2 * backHeight;

        float z1 = baseZ - curveDepth * (t1 * t1);
        float z2 = baseZ - curveDepth * (t2 * t2);

        float dy = (y2 - y1);
        float dz = (z2 - z1);
        float len = sqrtf(dy*dy + dz*dz);
        float ny = dy/len;
        float nz = dz/len;

        float u1 = 0, u2 = 1;

        /* FRONT */
        glBegin(GL_QUADS);
        glNormal3f(0, ny, nz);
        glTexCoord2f(u1, t1);
        glVertex3f(-seatW/2, y1, z1+halfT);

        glTexCoord2f(u2, t1);
        glVertex3f( seatW/2, y1, z1+halfT);

        glTexCoord2f(u2, t2);
        glVertex3f( seatW/2, y2, z2+halfT);

        glTexCoord2f(u1, t2);
        glVertex3f(-seatW/2, y2, z2+halfT);
        glEnd();

        /* BACK */
        glBegin(GL_QUADS);
        glNormal3f(0, ny, -nz);
        glTexCoord2f(u1, t1);
        glVertex3f(-seatW/2, y1, z1-halfT);

        glTexCoord2f(u2, t1);
        glVertex3f( seatW/2, y1, z1-halfT);

        glTexCoord2f(u2, t2);
        glVertex3f( seatW/2, y2, z2-halfT);

        glTexCoord2f(u1, t2);
        glVertex3f(-seatW/2, y2, z2-halfT);
        glEnd();

        /* LEFT side */
        glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glTexCoord2f(0, t1); glVertex3f(-seatW/2, y1, z1-halfT);
        glTexCoord2f(1, t1); glVertex3f(-seatW/2, y1, z1+halfT);
        glTexCoord2f(1, t2); glVertex3f(-seatW/2, y2, z2+halfT);
        glTexCoord2f(0, t2); glVertex3f(-seatW/2, y2, z2-halfT);
        glEnd();

        /* RIGHT side */
        glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glTexCoord2f(0, t1); glVertex3f(seatW/2, y1, z1-halfT);
        glTexCoord2f(1, t1); glVertex3f(seatW/2, y1, z1+halfT);
        glTexCoord2f(1, t2); glVertex3f(seatW/2, y2, z2+halfT);
        glTexCoord2f(0, t2); glVertex3f(seatW/2, y2, z2-halfT);
        glEnd();
    }

    /* TOP CAP */
    float yTop = backBaseY + backHeight;
    float zTop = baseZ - curveDepth;

    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-seatW/2, yTop, zTop-halfT);
    glTexCoord2f(1,0); glVertex3f( seatW/2, yTop, zTop-halfT);
    glTexCoord2f(1,1); glVertex3f( seatW/2, yTop, zTop+halfT);
    glTexCoord2f(0,1); glVertex3f(-seatW/2, yTop, zTop+halfT);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// ============================================================
//  Object Collision Detection Helpers
// ============================================================
#include <math.h>
#include <stdbool.h>
#include "CSCIx229.h"

// External scene definitions (provided elsewhere)
extern SceneObject objects[];
extern int objectCount;

// ------------------------------------------------------------
// Compute a reasonable radius from the object's bounding box
// ------------------------------------------------------------
// static float computeRadius(SceneObject* obj)
// {
//     float width  = obj->bbox[1] - obj->bbox[0];
//     float depth  = obj->bbox[5] - obj->bbox[4];
//     float radius = 0.5f * fmaxf(width, depth);
//     return radius;
// }

// ------------------------------------------------------------
// Check if movingObj placed at (newX, newZ) collides with others
// ------------------------------------------------------------
#include <math.h>
#include <stdbool.h>
#include "CSCIx229.h"

extern SceneObject objects[];
extern int objectCount;

bool collidesWithAnyObject(SceneObject* movingObj, float newX, float newZ)
{
    float minX = newX + movingObj->bbox[0];
    float maxX = newX + movingObj->bbox[1];
    float minZ = newZ + movingObj->bbox[4];
    float maxZ = newZ + movingObj->bbox[5];

    for (int i = 0; i < objectCount; i++)
    {
        SceneObject* other = &objects[i];
        if (other == movingObj || !other->solid) continue;

        float oMinX = other->x + other->bbox[0];
        float oMaxX = other->x + other->bbox[1];
        float oMinZ = other->z + other->bbox[4];
        float oMaxZ = other->z + other->bbox[5];

        if (maxX > oMinX && minX < oMaxX &&
            maxZ > oMinZ && minZ < oMaxZ)
        {
            return true; // Collision detected
        }
    }

    return false; // No overlap
}

// ============================================================
//  Object Rotation Helper (updated to match header + struct)
// ============================================================
void rotateObject(SceneObject* obj, float angle)
{
    if (!obj) return;

    // Adjust rotation (in degrees)
    obj->rotation += angle;

    // Wrap within 0–360
    if (obj->rotation >= 360.0f) obj->rotation -= 360.0f;
    if (obj->rotation < 0.0f)    obj->rotation += 360.0f;
}


//-----------------------------------------------------------
//  NEW SNOWFLAKE COCKTAIL TABLE
//-----------------------------------------------------------
extern unsigned int cocktail2Tex;
extern unsigned int cocktail2LegTex;

void drawCocktailTable2(float x, float z)
{
    const int N = 8;
    const int ARC_STEPS = 12;
    const float R = 1.4f;
    const float CURVE_IN = 0.22f;
    const float THICK = 0.12f;

    const float HEIGHT = 4.0f;
    const float LEG_H  = HEIGHT;

    const float LEG_TOP_R = 0.10f;
    const float LEG_BOTTOM_R = 0.04f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    /* =========================================================
       1. TABLETOP TEXTURE  (cocktail2Tex)
       ========================================================= */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2Tex);

    float wedgeAngle = 2 * M_PI / N;

    /* ----------------------
       TOP FACE
       ----------------------*/
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float a0 = w * wedgeAngle;
        float a1 = (w + 1) * wedgeAngle;

        float prev_x = 0, prev_z = 0;

        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            float mid = fabs(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1 - mid);
            float r = R - curve;

            float xArc = r * cos(a);
            float zArc = r * sin(a);

            if (i > 0)
            {
                glNormal3f(0,1,0);

                glTexCoord2f(0.5f, 0.5f);
                glVertex3f(0, HEIGHT + THICK, 0);

                glTexCoord2f((cos(a0)+1)*0.5f, (sin(a0)+1)*0.5f);
                glVertex3f(prev_x, HEIGHT + THICK, prev_z);

                glTexCoord2f((cos(a)+1)*0.5f, (sin(a)+1)*0.5f);
                glVertex3f(xArc, HEIGHT + THICK, zArc);
            }

            prev_x = xArc;
            prev_z = zArc;
        }
    }
    glEnd();

    /* ----------------------
       SIDE WALL
       ----------------------*/
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N * ARC_STEPS; i++)
    {
        float a = (float)i / (N * ARC_STEPS) * (2 * M_PI);
        float wedgePos = fmod(a, wedgeAngle) / wedgeAngle;

        float mid = fabs(wedgePos - 0.5f) * 2.0f;
        float curve = CURVE_IN * (1 - mid);

        float r = R - curve;

        float x = r * cos(a);
        float z = r * sin(a);

        glNormal3f(cos(a),0,sin(a));

        glTexCoord2f((float)i/(N*ARC_STEPS), 1);
        glVertex3f(x, HEIGHT, z);

        glTexCoord2f((float)i/(N*ARC_STEPS), 0);
        glVertex3f(x, HEIGHT + THICK, z);
    }
    glEnd();

    /* ----------------------
       UNDERSIDE
       ----------------------*/
    glBegin(GL_TRIANGLES);
    for (int w = 0; w < N; w++)
    {
        float a0 = w * wedgeAngle;
        float a1 = (w + 1) * wedgeAngle;

        float prev_x = 0, prev_z = 0;

        for (int i = 0; i <= ARC_STEPS; i++)
        {
            float t = (float)i / ARC_STEPS;
            float a = a0 + t * (a1 - a0);

            float mid = fabs(t - 0.5f) * 2.0f;
            float curve = CURVE_IN * (1 - mid);
            float r = R - curve;

            float xArc = r * cos(a);
            float zArc = r * sin(a);

            if (i > 0)
            {
                glNormal3f(0,-1,0);
                glVertex3f(0, HEIGHT, 0);
                glVertex3f(prev_x, HEIGHT, prev_z);
                glVertex3f(xArc, HEIGHT, zArc);
            }

            prev_x = xArc;
            prev_z = zArc;
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    /* =========================================================
       2. LEG TEXTURE  (cocktail2LegTex)
       ========================================================= */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail2LegTex);

    /* ----------------------
       THREE LEGS
       ----------------------*/
    for (int i = 0; i < 3; i++)
    {
        float ang = i * 2 * M_PI / 3.0f;
        float lx = 0.8f * cos(ang);
        float lz = 0.8f * sin(ang);

        glPushMatrix();
        glTranslatef(lx, HEIGHT, lz);
        glRotatef(-10, sin(ang), 0, -cos(ang));

        glBegin(GL_QUAD_STRIP);
        for (int s = 0; s <= 24; s++)
        {
            float t = 2 * M_PI * s / 24.0f;

            float nx = cos(t);
            float nz = sin(t);

            glNormal3f(nx, 0, nz);

            // simple cylindrical UV mapping
            float u = (float)s / 24.0f;

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


void drawFrustum(float bottomRadius, float topRadius, float height, int slices)
{
    float angleStep = (2.0f * M_PI) / slices;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; i++)
    {
        float a0 = i * angleStep;
        float a1 = (i + 1) * angleStep;

        float x0b = bottomRadius * cos(a0);
        float z0b = bottomRadius * sin(a0);
        float x1b = bottomRadius * cos(a1);
        float z1b = bottomRadius * sin(a1);

        float x0t = topRadius * cos(a0);
        float z0t = topRadius * sin(a0);
        float x1t = topRadius * cos(a1);
        float z1t = topRadius * sin(a1);

        // texture coordinates (cylindrical)
        float u0 = (float)i / slices;
        float u1 = (float)(i + 1) / slices;

        // normals for smooth shading
        float nx0 = x0b + x0t;
        float nz0 = z0b + z0t;
        float len0 = sqrt(nx0 * nx0 + nz0 * nz0);
        nx0 /= len0; nz0 /= len0;

        float nx1 = x1b + x1t;
        float nz1 = z1b + z1t;
        float len1 = sqrt(nx1 * nx1 + nz1 * nz1);
        nx1 /= len1; nz1 /= len1;

        // --- Triangle 1 ---
        glNormal3f(nx0, 0, nz0);
        glTexCoord2f(u0, 0); glVertex3f(x0b, 0, z0b);
        glTexCoord2f(u0, 1); glVertex3f(x0t, height, z0t);
        glNormal3f(nx1, 0, nz1);
        glTexCoord2f(u1, 1); glVertex3f(x1t, height, z1t);

        // --- Triangle 2 ---
        glNormal3f(nx0, 0, nz0);
        glTexCoord2f(u0, 0); glVertex3f(x0b, 0, z0b);
        glNormal3f(nx1, 0, nz1);
        glTexCoord2f(u1, 1); glVertex3f(x1t, height, z1t);
        glTexCoord2f(u1, 0); glVertex3f(x1b, 0, z1b);
    }
    glEnd();
}

void drawCocktailTable3(float x, float z)
{
    int slices = 40;

    float bottomHeight = 2.3f;
    float topHeight    = 1.7f;       // bottom + top = 4.0f

    glPushMatrix();
    glTranslatef(x, 0, z);


    /* ===========================================
       1. LEG TEXTURE  (frustums use cocktail3LegTex)
       =========================================== */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3LegTex);

    //-----------------------------------------
    // Bottom frustum (base → mid)
    //-----------------------------------------
    drawFrustum(0.85f, 0.45f, bottomHeight, slices);

    glTranslatef(0, bottomHeight, 0);
    glDisable(GL_TEXTURE_2D);

    //-----------------------------------------
    // Upper frustum (mid → top)
    //-----------------------------------------
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);
    drawFrustum(0.45f, 1.35f, topHeight, slices);

    glDisable(GL_TEXTURE_2D);


    /* ===========================================
       2. TABLETOP TEXTURE  (cocktail3Tex)
       =========================================== */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktail3Tex);

    //-----------------------------------------
    // Top disk
    //-----------------------------------------
    float R = 1.35f;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, topHeight, 0);

    for (int i = 0; i <= slices; i++)
    {
        float ang = 2 * M_PI * i / slices;
        float cx  = cos(ang);
        float cz  = sin(ang);

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
    float width  = 2.6f;

    float tabletopY = 1.7f;
    float thickness = 0.15f;
    float legHeight = 1.6f;
    float legSize   = 0.25f;

    int segments = 140;

    float halfL = length * 0.5f;
    float halfW = width  * 0.5f;

    /* ============================
       DRAW LEGS  (with meetingTableLegTex)
       ============================ */
    glPushMatrix();
    glTranslatef(x, legHeight * 0.5f, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, meetingTableLegTex);
    glColor3f(1,1,1);

    float lx = halfL * 0.55f;
    float lz = halfW * 0.45f;

    glPushMatrix(); glTranslatef( lx, 0,  lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, 0,  lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef( lx, 0, -lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, 0, -lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();



    /* ============================
       TABLETOP (meetingTableTex)
       ============================ */
    glPushMatrix();
    glTranslatef(x, tabletopY, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, meetingTableTex);
    glColor3f(1,1,1);

    /* Precompute shape */
    static float px[200], pz[200];
    int N = segments;

    for (int i = 0; i < N; i++)
    {
        float t = (float)i / (N - 1);
        float angle = t * 2.0f * M_PI;

        float nx = cos(angle);
        float ny = sin(angle);

        float rx = halfL * (0.7f + 0.3f * fabs(ny));
        float ry = halfW;

        float bulge = 1.0f + 0.20f * exp(-pow((fabs(nx) - 0.3f) * 5.0f, 2.0f));
        float pinch = 1.0f - 0.08f * exp(-pow(nx * 4.0f, 2.0f));

        float finalWidth = ry * bulge * pinch;

        px[i] = rx * nx;
        pz[i] = finalWidth * ny;
    }


    /* --------------------------------
       TOP SURFACE (with texture)
       -------------------------------- */
    glBegin(GL_POLYGON);
    for (int i = 0; i < N; i++)
    {
        glNormal3f(0,1,0);
        glTexCoord2f((px[i]/length) + 0.5f, (pz[i]/width) + 0.5f);
        glVertex3f(px[i], 0, pz[i]);
    }
    glEnd();



    /* --------------------------------
       BOTTOM SURFACE
       -------------------------------- */
    glBegin(GL_POLYGON);
    for (int i = 0; i < N; i++)
    {
        glNormal3f(0,-1,0);
        glTexCoord2f((px[i]/length) + 0.5f, (pz[i]/width) + 0.5f);
        glVertex3f(px[i], -thickness, pz[i]);
    }
    glEnd();



    /* --------------------------------
       SIDE WALLS (with texture)
       -------------------------------- */
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N; i++)
    {
        int k = i % N;

        float sx = px[k];
        float sz = pz[k];

        float len = sqrt(sx*sx + sz*sz);
        float nx = sx / len;
        float nz = sz / len;

        glNormal3f(nx, 0, nz);

        float u = (float)i / N;  // unwrap texture
        float v1 = 0.0f;
        float v2 = 1.0f;

        glTexCoord2f(u, v1); glVertex3f(sx, 0,          sz);
        glTexCoord2f(u, v2); glVertex3f(sx, -thickness, sz);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/*******************************************************
 *                 CEILING LIGHT OBJECTS
 *******************************************************/

static void drawRope(float length)
{
    glColor3f(1.0, 1.0, 1.0);   // IMPORTANT: leave color white so texture isn't tinted

    glPushMatrix();

    // Enable rope texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, threadTex);

    // Draw rope as a long thin cube
    glTranslatef(0, -length / 2.0f, 0);
    glScalef(0.05f, length, 0.05f);

    drawCube();   // MUST use textured cube implementation

    // Disable after drawing
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawStarShape()
{
    const float outerR = 1.0f;
    const float innerR = 0.55f;
    const float depth  = 0.35f;
    const int numPoints = 5;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, starTex);
    glColor3f(1,1,1);     // let texture show fully

    /* --------------------------
       FRONT FACE  (+Z)
    --------------------------- */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,1);

    // Center of textured fan
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, depth);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cos(ang);
        float y = r * sin(ang);

        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, depth);
    }
    glEnd();


    /* --------------------------
       BACK FACE (−Z)
    --------------------------- */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,-1);

    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, 0, -depth);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cos(ang);
        float y = r * sin(ang);

        float u = (x / outerR + 1.0f) * 0.5f;
        float v = (y / outerR + 1.0f) * 0.5f;

        glTexCoord2f(u, v);
        glVertex3f(x, y, -depth);
    }
    glEnd();


    /* --------------------------
       SIDE WALLS (textured)
    --------------------------- */
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= numPoints * 2; i++)
    {
        float ang = i * M_PI / numPoints;
        float r = (i % 2 == 0 ? outerR : innerR);

        float x = r * cos(ang);
        float y = r * sin(ang);

        float len = sqrtf(x*x + y*y);
        glNormal3f(x/len, y/len, 0);

        // wrap texture around the side walls
        float u = (float)i / (numPoints * 2);
        float v1 = 0.0f;
        float v2 = 1.0f;

        glTexCoord2f(u, v1); glVertex3f(x, y,  depth);
        glTexCoord2f(u, v2); glVertex3f(x, y, -depth);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawCloudShape()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTex);

    glColor3f(1.0f, 1.0f, 1.0f);  // show texture fully

    // center puff
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glScalef(1.4f, 1.2f, 1.0f);
        drawSphere(0.5f, 32, 32);
    glPopMatrix();

    // right big puff
    glPushMatrix();
        glTranslatef(0.65f, -0.05f, 0.0f);
        glScalef(1.2f, 1.0f, 1.0f);
        drawSphere(0.45f, 32, 32);
    glPopMatrix();

    // left big puff
    glPushMatrix();
        glTranslatef(-0.65f, -0.05f, 0.0f);
        glScalef(1.2f, 1.0f, 1.0f);
        drawSphere(0.45f, 32, 32);
    glPopMatrix();

    // lower-left puff
    glPushMatrix();
        glTranslatef(-0.35f, -0.35f, 0.0f);
        glScalef(0.9f, 0.8f, 1.0f);
        drawSphere(0.35f, 32, 32);
    glPopMatrix();

    // lower-right puff
    glPushMatrix();
        glTranslatef(0.35f, -0.35f, 0.0f);
        glScalef(0.9f, 0.8f, 1.0f);
        drawSphere(0.35f, 32, 32);
    glPopMatrix();

    // top-right puff
    glPushMatrix();
        glTranslatef(0.30f, 0.35f, 0.0f);
        glScalef(0.8f, 0.8f, 1.0f);
        drawSphere(0.30f, 32, 32);
    glPopMatrix();

    // top-left puff
    glPushMatrix();
        glTranslatef(-0.30f, 0.35f, 0.0f);
        glScalef(0.8f, 0.8f, 1.0f);
        drawSphere(0.30f, 32, 32);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}



static void drawMoonShape()
{
    float outerR  = 0.9f;
    float innerR  = 0.60f;
    float offset  = 0.35f;
    float depth   = 0.15f;
    int steps     = 300;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, moonTex);
    glColor3f(1,1,1);

    glPushMatrix();

    /* ============================
       FRONT FACE
       ============================ */
    glNormal3f(0,0,1);
    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a  = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        if ((ox*ox + oy*oy) > (ix*ix + iy*iy))
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


    /* ============================
       BACK FACE
       ============================ */
    glNormal3f(0,0,-1);
    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a  = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        if ((ox*ox + oy*oy) > (ix*ix + iy*iy))
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


    /* ============================
       OUTER WALLS
       ============================ */
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a  = (float)i / steps * 2.0f * M_PI;

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        if ((ox*ox + oy*oy) > (ix*ix + iy*iy))
        {
            float len = sqrtf(ox*ox + oy*oy);
            glNormal3f(ox/len, oy/len, 0);

            float u = (float)i / steps;   // wrap texture around edge
            float v1 = 0.0f;
            float v2 = 1.0f;

            glTexCoord2f(u, v1); glVertex3f(ox, oy,  depth);
            glTexCoord2f(u, v2); glVertex3f(ox, oy, -depth);
        }
    }

    glEnd();


    /* ============================
       INNER WALLS
       ============================ */
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a  = (float)i / steps * 2.0f * M_PI;

        float ix = innerR * cos(a) + offset;
        float iy = innerR * sin(a);

        float ox = outerR * cos(a);
        float oy = outerR * sin(a);

        if ((ox*ox + oy*oy) > (ix*ix + iy*iy))
        {
            float len = sqrtf((ix-offset)*(ix-offset) + iy*iy);
            float nx  = -(ix - offset) / len;
            float ny  = -iy / len;
            glNormal3f(nx, ny, 0);

            float u = (float)i / steps;
            float v1 = 0.0f;
            float v2 = 1.0f;

            glTexCoord2f(u, v1); glVertex3f(ix, iy, -depth);
            glTexCoord2f(u, v2); glVertex3f(ix, iy,  depth);
        }
    }

    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


void drawCeilingLight(float x, float y, float z, int shapeType)
{
    float ropeLength  = 2.0f;
    float shapeOffset = ropeLength + 0.45f;

    glPushMatrix();
    glTranslatef(x, y, z);

    // draw rope
    drawRope(ropeLength);

    // move to shape
    glPushMatrix();
    glTranslatef(0, -shapeOffset, 0);

    // draw shape only (no lighting)
    switch (shapeType)
    {
        case 0: drawStarShape();  break;
        case 1: drawCloudShape(); break;
        case 2: drawMoonShape();  break;
    }

    glPopMatrix();
    glPopMatrix();
}


void drawCeilingLights()
{
    // Example layout (modify as you like)

    // Row 1
    drawCeilingLight(-10, 14.5f, -10, 0);   // Star
    drawCeilingLight(  0, 14.5f, -10, 1);   // Cloud
    drawCeilingLight( 10, 14.5f, -10, 2);   // Moon

    // Row 2
    drawCeilingLight(-10, 14.5f,   0, 1);   // Cloud
    drawCeilingLight(  0, 14.5f,   0, 2);   // Moon
    drawCeilingLight( 10, 14.5f,   0, 0);   // Star

    // Row 3
    drawCeilingLight(-10, 14.5f,  10, 2);   // Moon
    drawCeilingLight(  0, 14.5f,  10, 0);   // Star
    drawCeilingLight( 10, 14.5f,  10, 1);   // Cloud
}

//----------------------------------
// Dimensions (easy to adjust)
//----------------------------------
//----------------------------------
// Dimensions (easy to adjust)
//----------------------------------
float CHAIR_SCALE = 1.8f;   // <— make chair taller overall

// raw globals (no formulas here!)
float BASE_THICK;
float CUSHION_THICK;
float LEG_HEIGHT;
float SEAT_HEIGHT;
float ROD_HEIGHT;
float ROD_BOTTOM;
float ROD_TOP;

float LEG_OFFSET_X = 0.45f;   // left-right
float LEG_OFFSET_Z = 0.35f;   // forward-back

//----------------------------------
// Cushion
//----------------------------------
void drawSeat()
{
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairCushionTex);
    glColor3f(1,1,1);   // allow texture to show fully

    float width = 1.0f;
    float halfW = width * 0.5f;

    float rectDepth = 0.55f;
    float radius = halfW;
    int steps = 48;

    float h = CUSHION_THICK;

    // Position the seat
    float bottomY = SEAT_HEIGHT - CUSHION_THICK;
    glTranslatef(0, bottomY + h*0.5f, 0);


    // =====================================================
    // 1. TOP FACE (textured)
    // =====================================================
    glBegin(GL_POLYGON);

    // front straight
    glTexCoord2f(0, 1); glVertex3f(-halfW, h/2, rectDepth);
    glTexCoord2f(1, 1); glVertex3f(+halfW, h/2, rectDepth);

    // curved back
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 3.14159f * t;
        float x = radius * cos(a);
        float z = -radius * sin(a);

        glTexCoord2f(t, 0); 
        glVertex3f(x, h/2, z);
    }

    glEnd();


    // =====================================================
    // 2. BOTTOM FACE (textured)
    // =====================================================
    glBegin(GL_POLYGON);

    glTexCoord2f(0, 1); glVertex3f(-halfW, -h/2, rectDepth);
    glTexCoord2f(1, 1); glVertex3f(+halfW, -h/2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 3.14159f * t;
        float x = radius * cos(a);
        float z = -radius * sin(a);

        glTexCoord2f(t, 0);
        glVertex3f(x, -h/2, z);
    }

    glEnd();


    // =====================================================
    // 3. FRONT WALL
    // =====================================================
    glBegin(GL_QUADS);

    glTexCoord2f(0,0); glVertex3f(-halfW, -h/2, rectDepth);
    glTexCoord2f(1,0); glVertex3f(+halfW, -h/2, rectDepth);
    glTexCoord2f(1,1); glVertex3f(+halfW, +h/2, rectDepth);
    glTexCoord2f(0,1); glVertex3f(-halfW, +h/2, rectDepth);

    glEnd();


    // =====================================================
    // 4. CURVED BACK WALL
    // =====================================================
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        float a = 3.14159f * t;
        float x = radius * cos(a);
        float z = -radius * sin(a);

        glTexCoord2f(t, 0); glVertex3f(x, -h/2, z);
        glTexCoord2f(t, 1); glVertex3f(x, +h/2, z);
    }

    glEnd();


    // =====================================================
    // 5. LEFT WALL
    // =====================================================
    glBegin(GL_QUADS);

    glTexCoord2f(0,0); glVertex3f(-halfW, -h/2, rectDepth);
    glTexCoord2f(1,0); glVertex3f(-halfW, -h/2, 0);
    glTexCoord2f(1,1); glVertex3f(-halfW, +h/2, 0);
    glTexCoord2f(0,1); glVertex3f(-halfW, +h/2, rectDepth);

    glEnd();


    // =====================================================
    // 6. RIGHT WALL
    // =====================================================
    glBegin(GL_QUADS);

    glTexCoord2f(0,0); glVertex3f(+halfW, -h/2, rectDepth);
    glTexCoord2f(1,0); glVertex3f(+halfW, -h/2, 0);
    glTexCoord2f(1,1); glVertex3f(+halfW, +h/2, 0);
    glTexCoord2f(0,1); glVertex3f(+halfW, +h/2, rectDepth);

    glEnd();


    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


//----------------------------------
// Wooden seat base UNDER cushion
//----------------------------------
void drawSeatBase()
{
    glPushMatrix();
    glColor3f(0.55f, 0.38f, 0.22f);   // darker wood

    // ------------------------
    // Dimensions
    // ------------------------
    float width = 1.12f;       // little wider than cushion
    float halfW = width * 0.5f;

    float rectDepth = 0.62f;   // slightly deeper
    float radius = halfW;      // semicircle radius

    int steps = 48;

    float h = BASE_THICK;      // base thickness

    // Position base under cushion
    float bottomY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;
    glTranslatef(0, bottomY + h*0.5f, 0);


    // =====================================================
    // 1. TOP FACE
    // =====================================================
    glBegin(GL_POLYGON);

    glVertex3f(-halfW, h/2, rectDepth);
    glVertex3f(+halfW, h/2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float a = 3.14159f * i / steps;
        float x = radius * cos(a);
        float z = -radius * sin(a);
        glVertex3f(x, h/2, z);
    }

    glEnd();


    // =====================================================
    // 2. BOTTOM FACE
    // =====================================================
    glBegin(GL_POLYGON);

    glVertex3f(-halfW, -h/2, rectDepth);
    glVertex3f(+halfW, -h/2, rectDepth);

    for (int i = 0; i <= steps; i++)
    {
        float a = 3.14159f * i / steps;
        float x = radius * cos(a);
        float z = -radius * sin(a);
        glVertex3f(x, -h/2, z);
    }

    glEnd();


    // =====================================================
    // 3. FRONT WALL
    // =====================================================
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h/2, rectDepth);
    glVertex3f(+halfW, -h/2, rectDepth);
    glVertex3f(+halfW, +h/2, rectDepth);
    glVertex3f(-halfW, +h/2, rectDepth);
    glEnd();


    // =====================================================
    // 4. CURVED BACK WALL
    // =====================================================
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= steps; i++)
    {
        float a = 3.14159f * i / steps;
        float x = radius * cos(a);
        float z = -radius * sin(a);

        glVertex3f(x, -h/2, z);
        glVertex3f(x, +h/2, z);
    }

    glEnd();


    // =====================================================
    // 5. LEFT SIDE WALL
    // =====================================================
    glBegin(GL_QUADS);
    glVertex3f(-halfW, -h/2, rectDepth);
    glVertex3f(-halfW, +h/2, rectDepth);
    glVertex3f(-halfW, +h/2, 0);
    glVertex3f(-halfW, -h/2, 0);
    glEnd();


    // =====================================================
    // 6. RIGHT SIDE WALL
    // =====================================================
    glBegin(GL_QUADS);
    glVertex3f(+halfW, -h/2, rectDepth);
    glVertex3f(+halfW, +h/2, rectDepth);
    glVertex3f(+halfW, +h/2, 0);
    glVertex3f(+halfW, -h/2, 0);
    glEnd();


    glPopMatrix();
}


//----------------------------------
// Legs (start from bottom of base)
//----------------------------------
void drawLegN(float x, float z)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    float legStartY = SEAT_HEIGHT - CUSHION_THICK - BASE_THICK;

    glTranslatef(x, legStartY - LEG_HEIGHT/2, z);
    glScalef(0.12f, LEG_HEIGHT, 0.12f);

    drawCube();  // <-- now using custom cube

    glPopMatrix();
}

//----------------------------------
// Horizontal bar between legs
//----------------------------------
void drawFootBar(float x1, float z1, float x2, float z2, float y)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    // midpoint of the bar
    glTranslatef((x1 + x2) * 0.5f, y, (z1 + z2) * 0.5f);

    float dx = x2 - x1;
    float dz = z2 - z1;

    float len = sqrt(dx*dx + dz*dz);

    // orientation around Y
    float ang = atan2(dx, dz) * 57.2958f;   // convert to degrees
    glRotatef(ang, 0, 1, 0);

    // scale to make it a bar
    glScalef(len, 0.08f, 0.08f);

    drawCube();   // ⬅ replaces glutSolidCube(1.0)

    glPopMatrix();
}


//----------------------------------
// Backrest
//----------------------------------
// ----------------------------------
// Curved, thick, solid wooden backrest
// ----------------------------------
void drawCurvedBackrest() {

    float rodBottom = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    float rodHeight = 0.75f * CHAIR_SCALE;
    float rodTop = rodBottom + rodHeight;

    float backrestZ = -0.1f;

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairBackTex);
    glColor3f(1,1,1);   // allow full texture color

    glTranslatef(0.0f, rodTop, backrestZ);
    glRotatef(-10, 1, 0, 0);
    glRotatef(180, 0, 1, 0);

    float radius    = 0.65f;
    float height    = 0.35f;
    float thickness = 0.30f;
    int   steps     = 160;

    for (int j = 0; j < steps; j++) {

        float t0 = (float)j / steps;        // texture U
        float t1 = (float)(j+1) / steps;

        float a1 = (j - steps/2) * 0.017453f;
        float a2 = (j - steps/2 + 1) * 0.017453f;

        float x1 = radius * sin(a1);
        float z1 = radius * cos(a1);
        float x2 = radius * sin(a2);
        float z2 = radius * cos(a2);

        glBegin(GL_QUADS);

        /* ---------------- FRONT PLATE ---------------- */
        glTexCoord2f(t0, 1); glVertex3f(x1, 0,      z1);
        glTexCoord2f(t1, 1); glVertex3f(x2, 0,      z2);
        glTexCoord2f(t1, 0); glVertex3f(x2, height, z2);
        glTexCoord2f(t0, 0); glVertex3f(x1, height, z1);

        /* ---------------- BACK PLATE ---------------- */
        glTexCoord2f(t0, 1); glVertex3f(x1, 0,      z1 - thickness);
        glTexCoord2f(t1, 1); glVertex3f(x2, 0,      z2 - thickness);
        glTexCoord2f(t1, 0); glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0); glVertex3f(x1, height, z1 - thickness);

        /* ---------------- TOP EDGE ---------------- */
        glTexCoord2f(t0, 1); glVertex3f(x1, height, z1);
        glTexCoord2f(t1, 1); glVertex3f(x2, height, z2);
        glTexCoord2f(t1, 0); glVertex3f(x2, height, z2 - thickness);
        glTexCoord2f(t0, 0); glVertex3f(x1, height, z1 - thickness);

        /* ---------------- BOTTOM EDGE ---------------- */
        glTexCoord2f(t0, 1); glVertex3f(x1, 0,      z1);
        glTexCoord2f(t1, 1); glVertex3f(x2, 0,      z2);
        glTexCoord2f(t1, 0); glVertex3f(x2, 0,      z2 - thickness);
        glTexCoord2f(t0, 0); glVertex3f(x1, 0,      z1 - thickness);

        /* LEFT SIDE */
        if (j == 0) {
            glTexCoord2f(0,1); glVertex3f(x1, 0,      z1);
            glTexCoord2f(1,1); glVertex3f(x1, 0,      z1 - thickness);
            glTexCoord2f(1,0); glVertex3f(x1, height, z1 - thickness);
            glTexCoord2f(0,0); glVertex3f(x1, height, z1);
        }

        /* RIGHT SIDE */
        if (j == steps - 1) {
            glTexCoord2f(0,1); glVertex3f(x2, 0,      z2);
            glTexCoord2f(1,1); glVertex3f(x2, 0,      z2 - thickness);
            glTexCoord2f(1,0); glVertex3f(x2, height, z2 - thickness);
            glTexCoord2f(0,0); glVertex3f(x2, height, z2);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}



void drawBackrestRod(float x, float z)
{
    glPushMatrix();
    glColor3f(0.45f, 0.30f, 0.18f);

    // Correct vertical placement
    glTranslatef(x, ROD_BOTTOM + ROD_HEIGHT * 0.5f, z);

    // Scale into long vertical rod
    glScalef(0.08f, ROD_HEIGHT, 0.08f);

    drawCube();   // Replaces glutSolidCube(1.0)

    glPopMatrix();
}




//----------------------------------
// Full chair
//----------------------------------
void drawBarChair()
{
    // ----------------------------------
    // SCALE ALL DIMENSIONS HERE
    // ----------------------------------
    BASE_THICK     = 0.15f * CHAIR_SCALE;
    CUSHION_THICK  = 0.18f * CHAIR_SCALE;
    LEG_HEIGHT     = 1.2f  * CHAIR_SCALE;

    ROD_HEIGHT = 0.75f * CHAIR_SCALE;
    ROD_BOTTOM = SEAT_HEIGHT - CUSHION_THICK + 0.02f;
    ROD_TOP    = ROD_BOTTOM + ROD_HEIGHT;

    SEAT_HEIGHT = LEG_HEIGHT + BASE_THICK + CUSHION_THICK;

    glPushMatrix();


    /* ============================
       1. SEAT BASE (wood)
       ============================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);
    drawSeatBase();
    glDisable(GL_TEXTURE_2D);


    /* ============================
       2. SEAT CUSHION (leather)
       ============================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairCushionTex);
    drawSeat();
    glDisable(GL_TEXTURE_2D);


    /* ============================
       3. LEGS (wood)
       ============================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);

    drawLegN(+LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, +LEG_OFFSET_Z);
    drawLegN(+LEG_OFFSET_X, -LEG_OFFSET_Z);
    drawLegN(-LEG_OFFSET_X, -LEG_OFFSET_Z);

    glDisable(GL_TEXTURE_2D);


    /* ============================
       4. BACKREST RODS (wood)
       ============================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairWoodTex);

    drawBackrestRod(0.0f,  -0.50f);   // center
    drawBackrestRod(+0.20f, -0.50f);  // right
    drawBackrestRod(-0.20f, -0.50f);  // left

    glDisable(GL_TEXTURE_2D);


    /* ============================
       5. CURVED BACKREST (leather)
       ============================ */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barChairBackTex);
    drawCurvedBackrest();
    glDisable(GL_TEXTURE_2D);


    glPopMatrix();
}



//----------------------------------
// Placement near cocktail table
//----------------------------------
//----------------------------------
// Placement near cocktail table
//----------------------------------
void drawBarChairObj(float x, float z)
{
    glPushMatrix();

    float Y_OFFSET = 0.0f;    // <-- FIX: lift chair onto floor
                            // adjust 1.0 , 1.2 , 1.4 until perfect

    glTranslatef(x, Y_OFFSET, z);
    glScalef(1.0f, 1.2f, 1.0f);

    drawBarChair();

    glPopMatrix();
}

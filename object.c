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

//
// Draw one cylindrical leg standing vertically (Y-axis)
//
static void drawLeg(float x, float z, float height, float radius)
{
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);  // <-- ENABLE TEXTURE COORDS

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(-90, 1, 0, 0);

    glColor3f(1,1,1);  // let texture show fully
    gluCylinder(q, radius, radius, height, 32, 4);

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(q);
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

    glutSolidSphere(0.12, 20, 20);

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

        glutSolidSphere(0.25,20,20);

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
void drawCocktailTable2(float x, float z)
{
    const int N = 8;
    const int ARC_STEPS = 12;
    const float R = 1.4f;
    const float CURVE_IN = 0.22f;
    const float THICK = 0.12f;

    // MATCH CocktailTable1 height
    const float HEIGHT = 4.0f;    
    const float LEG_H  = HEIGHT;

    const float LEG_TOP_R = 0.10f;
    const float LEG_BOTTOM_R = 0.04f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    //------------------------------------------------
    // 1. SNOWFLAKE TOP — now at HEIGHT + THICK
    //------------------------------------------------
    float wedgeAngle = 2 * M_PI / N;

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

                glTexCoord2f(0.5f,0.5f);
                glVertex3f(0, HEIGHT + THICK, 0);

                glTexCoord2f((cos(a0)+1)/2, (sin(a0)+1)/2);
                glVertex3f(prev_x, HEIGHT + THICK, prev_z);

                glTexCoord2f((cos(a)+1)/2, (sin(a)+1)/2);
                glVertex3f(xArc, HEIGHT + THICK, zArc);
            }

            prev_x = xArc;
            prev_z = zArc;
        }
    }
    glEnd();

    //------------------------------------------------
    // 2. Tabletop side wall
    //------------------------------------------------
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= N * ARC_STEPS; i++)
    {
        float a = (float)i / (N * ARC_STEPS) * 2 * M_PI;

        float wedgePos = fmod(a, wedgeAngle) / wedgeAngle;
        float mid = fabs(wedgePos - 0.5f) * 2.0f;
        float curve = CURVE_IN * (1 - mid);

        float r = R - curve;
        float x = r * cos(a);
        float z = r * sin(a);

        glNormal3f(cos(a), 0, sin(a));
        glTexCoord2f((float)i/(N*ARC_STEPS), 1);
        glVertex3f(x, HEIGHT, z);

        glTexCoord2f((float)i/(N*ARC_STEPS), 0);
        glVertex3f(x, HEIGHT + THICK, z);
    }
    glEnd();

    //------------------------------------------------
    // 3. Underside
    //------------------------------------------------
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

    //------------------------------------------------
    // 4. Three tapered legs — EXACT SAME HEIGHT
    //------------------------------------------------
    for (int i = 0; i < 3; i++)
    {
        float ang = i * 2 * M_PI / 3;
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

            glNormal3f(nx,0,nz);
            glVertex3f(nx * LEG_BOTTOM_R, -LEG_H, nz * LEG_BOTTOM_R);
            glVertex3f(nx * LEG_TOP_R, 0, nz * LEG_TOP_R);
        }
        glEnd();

        glPopMatrix();
    }

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
    glPushMatrix();
    glTranslatef(x, 0, z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cocktailTableTex);

    int slices = 40;

    //-----------------------------------------
    // Match CocktailTable height = 4.0f total
    //-----------------------------------------
    float bottomHeight = 2.3f;
    float topHeight    = 1.7f;       // together = 4.0f

    //-----------------------------------------
    // Bottom frustum
    //-----------------------------------------
    drawFrustum(0.85f, 0.45f, bottomHeight, slices);

    glTranslatef(0, bottomHeight, 0);

    //-----------------------------------------
    // Upper frustum
    //-----------------------------------------
    drawFrustum(0.45f, 1.35f, topHeight, slices);

    //-----------------------------------------
    // Top disk
    //-----------------------------------------
    float R = 1.35f;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glTexCoord2f(0.5f,0.5f);
    glVertex3f(0, topHeight, 0);

    for (int i = 0; i <= slices; i++)
    {
        float ang = 2 * M_PI * i / slices;
        float cx = cos(ang);
        float cz = sin(ang);

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

    float tabletopY = 2.0f;      // top height
    float thickness = 0.15f;     // solid thickness
    float legHeight = tabletopY; 
    float legSize   = 0.25f;     // square legs

    int segments = 140;

    float halfL = length * 0.5f;
    float halfW = width  * 0.5f;


    /* ============================
       DRAW LEGS
       ============================ */
    glPushMatrix();
    glTranslatef(x, legHeight * 0.5f, z);  // center legs

    glColor3f(0.6f, 0.45f, 0.3f); // darker wood

    float lx = halfL * 0.55f;
float lz = halfW * 0.45f;

    // 4 legs
    glPushMatrix(); glTranslatef( lx, 0,  lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, 0,  lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef( lx, 0, -lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, 0, -lz); drawCuboid(legSize, legHeight, legSize); glPopMatrix();

    glPopMatrix();


    /* ============================
       TABLETOP (solid)
       ============================ */
    glPushMatrix();
    glTranslatef(x, tabletopY, z);

    glColor3f(0.88f, 0.78f, 0.62f);  // light wood color


    /* --------------------------------
       1. Precompute shape points
       -------------------------------- */
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
       2. TOP SURFACE
       -------------------------------- */
    glBegin(GL_POLYGON);
    for (int i = 0; i < N; i++)
    {
        glNormal3f(0,1,0);
        glVertex3f(px[i], 0, pz[i]);
    }
    glEnd();


    /* --------------------------------
       3. BOTTOM SURFACE
       -------------------------------- */
    glBegin(GL_POLYGON);
    for (int i = 0; i < N; i++)
    {
        glNormal3f(0,-1,0);
        glVertex3f(px[i], -thickness, pz[i]);
    }
    glEnd();


    /* --------------------------------
       4. SIDE WALLS (solid thickness)
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

        glVertex3f(sx, 0,          sz);   // top edge
        glVertex3f(sx, -thickness, sz);   // bottom edge
    }

    glEnd();

    glPopMatrix();
}

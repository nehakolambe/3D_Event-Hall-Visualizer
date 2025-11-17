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
    gluQuadricNormals(q, GLU_SMOOTH);   // <-- IMPORTANT: adds smooth normals

    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.35, 0.2, 0.1);
    gluCylinder(q, radius, radius, height, 32, 4);
    glPopMatrix();

    gluDeleteQuadric(q);
}


//
// Draw a circular disk (for table top or base)
//
static void drawDisk(float radius, float y, float thickness)
{
    int segs = 48;
    float topY = y + thickness;

    /* ===== TOP FACE ===== */
    glColor3f(0.55, 0.27, 0.07);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);     // normal for all top vertices
    glVertex3f(0, topY, 0);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        glVertex3f(radius * cos(angle), topY, radius * sin(angle));
    }
    glEnd();


    /* ===== BOTTOM FACE ===== */
    glColor3f(0.35, 0.18, 0.05);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);    // Bottom normal
    glVertex3f(0, y, 0);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        glVertex3f(radius * cos(angle), y, radius * sin(angle));
    }
    glEnd();

    /* ===== SIDE WALL ===== */
    glColor3f(0.45, 0.23, 0.1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glNormal3f(cos(angle), 0, sin(angle));  // radial outward normal

        glVertex3f(x, y, z);
        glVertex3f(x, topY, z);
    }
    glEnd();
}

static void drawCuboid(float width, float height, float depth)
{
    float w = width  / 2.0f;
    float h = height / 2.0f;
    float d = depth  / 2.0f;

    glBegin(GL_QUADS);

    // FRONT (+Z)
    glNormal3f(0,0,1);
    glVertex3f(-w,-h, d); glVertex3f( w,-h, d);
    glVertex3f( w, h, d); glVertex3f(-w, h, d);

    // BACK (−Z)
    glNormal3f(0,0,-1);
    glVertex3f( w,-h,-d); glVertex3f(-w,-h,-d);
    glVertex3f(-w, h,-d); glVertex3f( w, h,-d);

    // LEFT (−X)
    glNormal3f(-1,0,0);
    glVertex3f(-w,-h,-d); glVertex3f(-w,-h, d);
    glVertex3f(-w, h, d); glVertex3f(-w, h,-d);

    // RIGHT (+X)
    glNormal3f(1,0,0);
    glVertex3f( w,-h, d); glVertex3f( w,-h,-d);
    glVertex3f( w, h,-d); glVertex3f( w, h, d);

    // TOP (+Y)
    glNormal3f(0,1,0);
    glVertex3f(-w, h,-d); glVertex3f(-w, h, d);
    glVertex3f( w, h, d); glVertex3f( w, h,-d);

    // BOTTOM (−Y)
    glNormal3f(0,-1,0);
    glVertex3f(-w,-h, d); glVertex3f( w,-h, d);
    glVertex3f( w,-h,-d); glVertex3f(-w,-h,-d);

    glEnd();
}


//
// Draw a rectangular table with rounded corners (no GLU/GLUT)
//
//
// Draw a rectangular table with rounded corners WITH NORMALS
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

    /*===============================
      TOP SURFACE  (flat normal +Y)
    ===============================*/
    glColor3f(0.55f,0.27f,0.07f);
    glNormal3f(0,1,0);

    glBegin(GL_QUADS);
    glVertex3f(-innerX, yTop, -innerZ);
    glVertex3f( innerX, yTop, -innerZ);
    glVertex3f( innerX, yTop,  innerZ);
    glVertex3f(-innerX, yTop,  innerZ);
    glEnd();


    /*===============================
      FLAT EDGE PADS (flat normals)
    ===============================*/
    glBegin(GL_QUADS);

    // front
    glNormal3f(0,1,0);
    glVertex3f(-innerX, yTop,  innerZ);
    glVertex3f( innerX, yTop,  innerZ);
    glVertex3f( innerX, yTop,  innerZ + radius);
    glVertex3f(-innerX, yTop,  innerZ + radius);

    // back
    glVertex3f(-innerX, yTop, -innerZ - radius);
    glVertex3f( innerX, yTop, -innerZ - radius);
    glVertex3f( innerX, yTop, -innerZ);
    glVertex3f(-innerX, yTop, -innerZ);

    // left
    glVertex3f(-innerX - radius, yTop, -innerZ);
    glVertex3f(-innerX,          yTop, -innerZ);
    glVertex3f(-innerX,          yTop,  innerZ);
    glVertex3f(-innerX - radius, yTop,  innerZ);

    // right
    glVertex3f(innerX,          yTop, -innerZ);
    glVertex3f(innerX + radius, yTop, -innerZ);
    glVertex3f(innerX + radius, yTop,  innerZ);
    glVertex3f(innerX,          yTop,  innerZ);

    glEnd();


    /*===============================
      ROUNDED CORNERS (smooth normals)
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
        glVertex3f(cx, yTop, cz);

        for(int i=0;i<=segs;i++){
            float a = (start + i*(end-start)/segs) * M_PI/180.0f;
            float xPos = cx + radius*cos(a);
            float zPos = cz + radius*sin(a);
            glVertex3f(xPos, yTop, zPos);
        }
        glEnd();
    }


    /*===============================
      CURVED SIDE WALLS (smooth normals)
    ===============================*/
    glColor3f(0.45f,0.23f,0.1f);

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

            // Smooth outward normal
            float nx = cos(a);
            float nz = sin(a);
            glNormal3f(nx,0,nz);

            glVertex3f(xPos,yBottom,zPos);
            glVertex3f(xPos,yTop,   zPos);
        }
        glEnd();
    }


    /*===============================
      FLAT SIDE WALLS (flat normals)
    ===============================*/
    glBegin(GL_QUADS);

    // front
    glNormal3f(0,0,1);
    glVertex3f(-innerX, yBottom, innerZ+radius);
    glVertex3f( innerX, yBottom, innerZ+radius);
    glVertex3f( innerX, yTop,    innerZ+radius);
    glVertex3f(-innerX, yTop,    innerZ+radius);

    // back
    glNormal3f(0,0,-1);
    glVertex3f(-innerX, yBottom, -innerZ-radius);
    glVertex3f( innerX, yBottom, -innerZ-radius);
    glVertex3f( innerX, yTop,    -innerZ-radius);
    glVertex3f(-innerX, yTop,    -innerZ-radius);

    // left
    glNormal3f(-1,0,0);
    glVertex3f(-innerX-radius, yBottom, -innerZ);
    glVertex3f(-innerX-radius, yBottom,  innerZ);
    glVertex3f(-innerX-radius, yTop,     innerZ);
    glVertex3f(-innerX-radius, yTop,    -innerZ);

    // right
    glNormal3f(1,0,0);
    glVertex3f(innerX+radius, yBottom, -innerZ);
    glVertex3f(innerX+radius, yBottom,  innerZ);
    glVertex3f(innerX+radius, yTop,     innerZ);
    glVertex3f(innerX+radius, yTop,    -innerZ);

    glEnd();


    /*===============================
      LEGS (drawCuboid already has normals)
    ===============================*/
    float offsetX = innerX;
    float offsetZ = innerZ;

    float pos[4][2] = {
        { offsetX,  offsetZ},
        {-offsetX,  offsetZ},
        {-offsetX, -offsetZ},
        { offsetX, -offsetZ}
    };

    glColor3f(0.35f,0.2f,0.1f);
    for(int i=0;i<4;i++){
        glPushMatrix();
        glTranslatef(pos[i][0], legHeight/2, pos[i][1]);
        drawCuboid(legSize, legHeight, legSize);
        glPopMatrix();
    }

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

    // Draw leg (central cylinder)
    drawLeg(0, 0, height, legRadius);

    // Draw circular tabletop
    drawDisk(topRadius, height, topThickness);

    // Optional base disk for stability
    glColor3f(0.35, 0.2, 0.1);
    drawDisk(0.6f, 0.0f, 0.05f);

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

    //
    // Determine the correct normal direction
    //
    float nx = 0.0, ny = 0.0, nz = -1.0;   // Door on FRONT wall (z = +20)
    // If door on BACK wall (z = -20) -> nz = +1
    // If door on LEFT wall          -> nx = +1, nz = 0
    // If door on RIGHT wall         -> nx = -1, nz = 0

    /* ======================================================
         DOOR FRAME (slightly in front of wall +0.01 offset)
       ====================================================== */
    glColor3f(0.3, 0.18, 0.05);

    glBegin(GL_QUADS);

    // ---- Left frame ----
    glNormal3f(nx, ny, nz);
    glVertex3f(x - halfW - 0.1, yBottom, z + 0.01);
    glVertex3f(x - halfW,       yBottom, z + 0.01);
    glVertex3f(x - halfW,       yTop,    z + 0.01);
    glVertex3f(x - halfW - 0.1, yTop,    z + 0.01);

    // ---- Right frame ----
    glNormal3f(nx, ny, nz);
    glVertex3f(x + halfW,       yBottom, z + 0.01);
    glVertex3f(x + halfW + 0.1, yBottom, z + 0.01);
    glVertex3f(x + halfW + 0.1, yTop,    z + 0.01);
    glVertex3f(x + halfW,       yTop,    z + 0.01);

    // ---- Top frame ----
    glNormal3f(nx, ny, nz);
    glVertex3f(x - halfW - 0.1, yTop, z + 0.01);
    glVertex3f(x + halfW + 0.1, yTop, z + 0.01);
    glVertex3f(x + halfW + 0.1, yTop + 0.1, z + 0.01);
    glVertex3f(x - halfW - 0.1, yTop + 0.1, z + 0.01);

    glEnd();


    /* ======================================================
                   DOOR PANEL (wood)
       ====================================================== */
    glColor3f(0.45, 0.23, 0.10);

    glBegin(GL_QUADS);
    glNormal3f(nx, ny, nz);
    glVertex3f(x - halfW, yBottom, z);
    glVertex3f(x + halfW, yBottom, z);
    glVertex3f(x + halfW, yTop,    z);
    glVertex3f(x - halfW, yTop,    z);
    glEnd();


    /* ======================================================
                 DOOR KNOB (emissive metal)
       ====================================================== */
    glPushMatrix();
    glColor3f(0.8, 0.8, 0.6);
    glTranslatef(x + halfW - 0.3, yBottom + height * 0.5, z + 0.05);

    glutSolidSphere(0.08, 16, 16);

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

    glColor3f(0.0, 0.3, 0.8);

    glBegin(GL_QUADS);
    for (int i = 0; i < segH; i++)
    {
        float t1 = -angleH/2 + angleH * i     / segH;
        float t2 = -angleH/2 + angleH * (i+1) / segH;

        for (int j = 0; j < segV; j++)
        {
            float p1 = -angleV/2 + angleV * j     / segV;
            float p2 = -angleV/2 + angleV * (j+1) / segV;

            //
            // Precompute positions
            //
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

            //
            // Compute normals (smooth)
            //
            // Horizontal normal component from horizontal curvature
            float nx_t1 = sin(t1);
            float nz_t1 = cos(t1);

            float nx_t2 = sin(t2);
            float nz_t2 = cos(t2);

            // Vertical component from vertical curvature
            float ny_p1 = sin(p1);
            float ny_p2 = sin(p2);

            //
            // Draw the quad with 4 normals
            //
            glNormal3f(nx_t1, ny_p1, nz_t1);
            glVertex3f(x11, y11, z11);

            glNormal3f(nx_t2, ny_p1, nz_t2);
            glVertex3f(x21, y21, z21);

            glNormal3f(nx_t2, ny_p2, nz_t2);
            glVertex3f(x22, y22, z22);

            glNormal3f(nx_t1, ny_p2, nz_t1);
            glVertex3f(x12, y12, z12);
        }
    }
    glEnd();

    glPopMatrix();
}


void drawLamp(float xPos, float zPos) {
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

    /* ============================================
       BASE — bottom disk (normal = -Y)
       ============================================ */
    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float t1 = 2 * PI * i / segments;
        float t2 = 2 * PI * (i + 1) / segments;

        float x1 = baseRadius * cos(t1);
        float z1 = baseRadius * sin(t1);
        float x2 = baseRadius * cos(t2);
        float z2 = baseRadius * sin(t2);

        glNormal3f(0, -1, 0);   // base faces floor
        glVertex3f(0, 0, 0);
        glVertex3f(x1, 0, z1);
        glVertex3f(x2, 0, z2);
    }
    glEnd();

    /* ============================================
       POLE — vertical cylinder with smooth normals
       ============================================ */
    glColor3f(0.95f, 0.95f, 0.95f);
    for (int i = 0; i < segments; i++) {
        float t1 = 2 * PI * i / segments;
        float t2 = 2 * PI * (i + 1) / segments;

        float x1 = poleRadius * cos(t1);
        float z1 = poleRadius * sin(t1);
        float x2 = poleRadius * cos(t2);
        float z2 = poleRadius * sin(t2);

        float nx1 = cos(t1), nz1 = sin(t1);
        float nx2 = cos(t2), nz2 = sin(t2);

        glBegin(GL_QUADS);
        glNormal3f(nx1, 0, nz1); glVertex3f(x1, 0,        z1);
        glNormal3f(nx2, 0, nz2); glVertex3f(x2, 0,        z2);
        glNormal3f(nx2, 0, nz2); glVertex3f(x2, poleHeight, z2);
        glNormal3f(nx1, 0, nz1); glVertex3f(x1, poleHeight, z1);
        glEnd();
    }

    /* ============================================
       SHADE — truncated cone with smooth normals
       ============================================ */
    float shadeBottomY = poleHeight;
    float shadeTopY    = poleHeight + shadeHeight;

    glColor3f(0.95f, 0.95f, 0.85f);
    for (int i = 0; i < segments; i++) {
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

        // Smooth normals for cone surface
        float nx1 = cos(t1);
        float nz1 = sin(t1);
        float nx2 = cos(t2);
        float nz2 = sin(t2);

        glBegin(GL_QUADS);
        glNormal3f(nx1, 0, nz1); glVertex3f(x1b, shadeBottomY, z1b);
        glNormal3f(nx2, 0, nz2); glVertex3f(x2b, shadeBottomY, z2b);
        glNormal3f(nx2, 0, nz2); glVertex3f(x2t, shadeTopY,    z2t);
        glNormal3f(nx1, 0, nz1); glVertex3f(x1t, shadeTopY,    z1t);
        glEnd();
    }

    /* ============================================
       SHADE TOP — disk (normal = +Y)
       ============================================ */
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float t1 = 2 * PI * i / segments;
        float t2 = 2 * PI * (i + 1) / segments;

        float x1 = shadeTopRadius * cos(t1);
        float z1 = shadeTopRadius * sin(t1);
        float x2 = shadeTopRadius * cos(t2);
        float z2 = shadeTopRadius * sin(t2);

        glNormal3f(0, 1, 0);   // top faces upward
        glVertex3f(0, shadeTopY, 0);
        glVertex3f(x1, shadeTopY, z1);
        glVertex3f(x2, shadeTopY, z2);
    }
    glEnd();

// ---- bulb inside the shade ----
if (lightState == 2)
{
    glPushMatrix();

    // place bulb inside shade
    float bulbY = shadeBottomY + 0.4f;

    glTranslatef(0.0f, bulbY, 0.0f);

    // emission glow
    float bulbEmission[] = {1.0, 0.9, 0.7, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION, bulbEmission);

    glutSolidSphere(0.25, 20, 20);

    float noEmission[] = {0,0,0,1};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    glPopMatrix();
}

    glPopMatrix();
}

void drawBanquetChair(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);

    // ==== Parameters ====
    float seatY = 1.0f;
    float thickness = 0.25f;   // shared thickness for seat and backrest
    float seatW = 1.0f;
    float seatD = 1.0f;
    float legH = 1.0f;

    float backHeight = 1.3f;
    float curveDepth = 0.25f;
    int segments = 20;

    // ==== Seat (solid rectangular block) ====
    glColor3f(0.9f, 0.9f, 0.8f);
    glBegin(GL_QUADS);
    // top
    glVertex3f(-seatW/2, seatY + thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY + thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY + thickness/2,  seatD/2);
    glVertex3f(-seatW/2, seatY + thickness/2,  seatD/2);

    // bottom
    glVertex3f(-seatW/2, seatY - thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY - thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY - thickness/2,  seatD/2);
    glVertex3f(-seatW/2, seatY - thickness/2,  seatD/2);

    // left
    glVertex3f(-seatW/2, seatY - thickness/2, -seatD/2);
    glVertex3f(-seatW/2, seatY + thickness/2, -seatD/2);
    glVertex3f(-seatW/2, seatY + thickness/2,  seatD/2);
    glVertex3f(-seatW/2, seatY - thickness/2,  seatD/2);

    // right
    glVertex3f(seatW/2, seatY - thickness/2, -seatD/2);
    glVertex3f(seatW/2, seatY + thickness/2, -seatD/2);
    glVertex3f(seatW/2, seatY + thickness/2,  seatD/2);
    glVertex3f(seatW/2, seatY - thickness/2,  seatD/2);

    // front
    glVertex3f(-seatW/2, seatY - thickness/2, seatD/2);
    glVertex3f(seatW/2,  seatY - thickness/2, seatD/2);
    glVertex3f(seatW/2,  seatY + thickness/2, seatD/2);
    glVertex3f(-seatW/2, seatY + thickness/2, seatD/2);

    // back
    glVertex3f(-seatW/2, seatY - thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY - thickness/2, -seatD/2);
    glVertex3f(seatW/2,  seatY + thickness/2, -seatD/2);
    glVertex3f(-seatW/2, seatY + thickness/2, -seatD/2);
    glEnd();

    // ==== Legs ====
    glColor3f(0.8f, 0.6f, 0.2f);
    float legW = 0.07f;
    float halfW = seatW/2 - 0.1f;
    float halfD = seatD/2 - 0.1f;

    float legTop = seatY - thickness/2;
    float legBottom = legTop - legH;

    for (int i = -1; i <= 1; i += 2)
    for (int j = -1; j <= 1; j += 2)
    {
        float lx = i * halfW;
        float lz = j * halfD;

        glBegin(GL_QUADS);
        // front
        glVertex3f(lx - legW/2, legBottom, lz - legW/2);
        glVertex3f(lx + legW/2, legBottom, lz - legW/2);
        glVertex3f(lx + legW/2, legTop,    lz - legW/2);
        glVertex3f(lx - legW/2, legTop,    lz - legW/2);

        // back
        glVertex3f(lx - legW/2, legBottom, lz + legW/2);
        glVertex3f(lx + legW/2, legBottom, lz + legW/2);
        glVertex3f(lx + legW/2, legTop,    lz + legW/2);
        glVertex3f(lx - legW/2, legTop,    lz + legW/2);

        // left
        glVertex3f(lx - legW/2, legBottom, lz - legW/2);
        glVertex3f(lx - legW/2, legBottom, lz + legW/2);
        glVertex3f(lx - legW/2, legTop,    lz + legW/2);
        glVertex3f(lx - legW/2, legTop,    lz - legW/2);

        // right
        glVertex3f(lx + legW/2, legBottom, lz - legW/2);
        glVertex3f(lx + legW/2, legBottom, lz + legW/2);
        glVertex3f(lx + legW/2, legTop,    lz + legW/2);
        glVertex3f(lx + legW/2, legTop,    lz - legW/2);
        glEnd();
    }

/* ====================================================
      CURVED BACKREST — WITH FULL THICKNESS + NORMALS
   ==================================================== */

glColor3f(0.9f, 0.9f, 0.8f);

float baseZ = -seatD/2;      
float halfT = thickness / 2.0f;
float backBaseY = seatY - thickness/2;

// each segment draws front, back, left, right
for (int i = 0; i < segments; i++)
{
    float t1 = (float)i / segments;
    float t2 = (float)(i + 1) / segments;

    float y1 = backBaseY + t1 * backHeight;
    float y2 = backBaseY + t2 * backHeight;

    float z1 = baseZ - curveDepth * (t1 * t1);
    float z2 = baseZ - curveDepth * (t2 * t2);

    float dy = (y2 - y1);
    float dz = (z2 - z1);
    float len = sqrtf(dy*dy + dz*dz);
    float ny = dy/len;
    float nz = dz/len;

    // ===== FRONT FACE =====
    glBegin(GL_QUADS);
    glNormal3f(0, ny, nz);
    glVertex3f(-seatW/2, y1, z1 + halfT);
    glVertex3f( seatW/2, y1, z1 + halfT);
    glVertex3f( seatW/2, y2, z2 + halfT);
    glVertex3f(-seatW/2, y2, z2 + halfT);
    glEnd();

    // ===== BACK FACE =====
    glBegin(GL_QUADS);
    glNormal3f(0, ny, -nz);
    glVertex3f(-seatW/2, y1, z1 - halfT);
    glVertex3f( seatW/2, y1, z1 - halfT);
    glVertex3f( seatW/2, y2, z2 - halfT);
    glVertex3f(-seatW/2, y2, z2 - halfT);
    glEnd();

    // ===== LEFT SIDE =====
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(-seatW/2, y1, z1 - halfT);
    glVertex3f(-seatW/2, y1, z1 + halfT);
    glVertex3f(-seatW/2, y2, z2 + halfT);
    glVertex3f(-seatW/2, y2, z2 - halfT);
    glEnd();

    // ===== RIGHT SIDE =====
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(seatW/2, y1, z1 - halfT);
    glVertex3f(seatW/2, y1, z1 + halfT);
    glVertex3f(seatW/2, y2, z2 + halfT);
    glVertex3f(seatW/2, y2, z2 - halfT);
    glEnd();
}

// ====== TOP CAP (with normal pointing up) ======
float yTop = backBaseY + backHeight;
float zTop = baseZ - curveDepth;

glBegin(GL_QUADS);
glNormal3f(0,1,0);
glVertex3f(-seatW/2, yTop, zTop - halfT);
glVertex3f( seatW/2, yTop, zTop - halfT);
glVertex3f( seatW/2, yTop, zTop + halfT);
glVertex3f(-seatW/2, yTop, zTop + halfT);
glEnd();


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

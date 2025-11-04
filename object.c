#include "CSCIx229.h"

//
// Draw a rectangular tabletop with slightly rounded corners
//
static void drawRoundedTop(float length, float width, float y, float thickness, float radius)
{
    float hl = length / 2;
    float hw = width / 2;
    float topY = y + thickness;

    glColor3f(0.55, 0.27, 0.07); // wood color

    // Top surface
    glBegin(GL_QUADS);
    glVertex3f(-hl, topY, -hw);
    glVertex3f( hl, topY, -hw);
    glVertex3f( hl, topY,  hw);
    glVertex3f(-hl, topY,  hw);
    glEnd();

    // Side faces (tabletop thickness)
    glColor3f(0.45, 0.23, 0.1);
    glBegin(GL_QUADS);
    // front
    glVertex3f(-hl, y,  hw);
    glVertex3f( hl, y,  hw);
    glVertex3f( hl, topY,  hw);
    glVertex3f(-hl, topY,  hw);
    // back
    glVertex3f(-hl, y, -hw);
    glVertex3f( hl, y, -hw);
    glVertex3f( hl, topY, -hw);
    glVertex3f(-hl, topY, -hw);
    // left
    glVertex3f(-hl, y, -hw);
    glVertex3f(-hl, y,  hw);
    glVertex3f(-hl, topY,  hw);
    glVertex3f(-hl, topY, -hw);
    // right
    glVertex3f( hl, y, -hw);
    glVertex3f( hl, y,  hw);
    glVertex3f( hl, topY,  hw);
    glVertex3f( hl, topY, -hw);
    glEnd();
}

//
// Draw one cylindrical leg standing vertically (Y-axis)
//
static void drawLeg(float x, float z, float height, float radius)
{
    GLUquadric* q = gluNewQuadric();

    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.35, 0.2, 0.1);
    gluCylinder(q, radius, radius, height, 20, 2);
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

    glColor3f(0.55, 0.27, 0.07); // wood color
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, topY, 0);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        glVertex3f(radius * cos(angle), topY, radius * sin(angle));
    }
    glEnd();

    // Side face (thickness)
    glColor3f(0.45, 0.23, 0.1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++)
    {
        float angle = 2 * M_PI * i / segs;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glVertex3f(x, y, z);
        glVertex3f(x, topY, z);
    }
    glEnd();
}

static void drawCuboid(float width, float height, float depth)
{
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    glBegin(GL_QUADS);
    // front
    glVertex3f(-w, -h,  d); glVertex3f( w, -h,  d);
    glVertex3f( w,  h,  d); glVertex3f(-w,  h,  d);
    // back
    glVertex3f(-w, -h, -d); glVertex3f(-w,  h, -d);
    glVertex3f( w,  h, -d); glVertex3f( w, -h, -d);
    // left
    glVertex3f(-w, -h, -d); glVertex3f(-w, -h,  d);
    glVertex3f(-w,  h,  d); glVertex3f(-w,  h, -d);
    // right
    glVertex3f( w, -h, -d); glVertex3f( w,  h, -d);
    glVertex3f( w,  h,  d); glVertex3f( w, -h,  d);
    // top
    glVertex3f(-w,  h, -d); glVertex3f(-w,  h,  d);
    glVertex3f( w,  h,  d); glVertex3f( w,  h, -d);
    // bottom
    glVertex3f(-w, -h, -d); glVertex3f( w, -h, -d);
    glVertex3f( w, -h,  d); glVertex3f(-w, -h,  d);
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
    float tabletopThickness = 0.15f;
    float legHeight = tabletopY;
    float legSize = 0.25f;
    float topLength = 4.0f;
    float topWidth  = 2.0f;
    float radius    = 0.3f;
    int segs        = 20;

    float yBottom = tabletopY;
    float yTop = tabletopY + tabletopThickness;
    float innerX = topLength / 2 - radius;
    float innerZ = topWidth  / 2 - radius;

    // ---------- TOP SURFACE ----------
    glColor3f(0.55f, 0.27f, 0.07f);

    // Central rectangle
    glBegin(GL_QUADS);
    glVertex3f(-innerX, yTop, -innerZ);
    glVertex3f( innerX, yTop, -innerZ);
    glVertex3f( innerX, yTop,  innerZ);
    glVertex3f(-innerX, yTop,  innerZ);
    glEnd();

    // Four edge rectangles (front/back/left/right)
    glBegin(GL_QUADS);
    // front edge
    glVertex3f(-innerX, yTop,  innerZ);
    glVertex3f( innerX, yTop,  innerZ);
    glVertex3f( innerX, yTop,  innerZ + radius);
    glVertex3f(-innerX, yTop,  innerZ + radius);
    // back edge
    glVertex3f(-innerX, yTop, -innerZ - radius);
    glVertex3f( innerX, yTop, -innerZ - radius);
    glVertex3f( innerX, yTop, -innerZ);
    glVertex3f(-innerX, yTop, -innerZ);
    // left edge
    glVertex3f(-innerX - radius, yTop, -innerZ);
    glVertex3f(-innerX,          yTop, -innerZ);
    glVertex3f(-innerX,          yTop,  innerZ);
    glVertex3f(-innerX - radius, yTop,  innerZ);
    // right edge
    glVertex3f(innerX,           yTop, -innerZ);
    glVertex3f(innerX + radius,  yTop, -innerZ);
    glVertex3f(innerX + radius,  yTop,  innerZ);
    glVertex3f(innerX,           yTop,  innerZ);
    glEnd();

// Four rounded corners (quarter-circle fans)
for (int corner = 0; corner < 4; corner++) {
    float cx = (corner == 0 || corner == 3) ?  innerX : -innerX;
    float cz = (corner < 2) ?  innerZ : -innerZ;

    float startAngle, endAngle;

    // Define arcs in CCW order around the table
    if (corner == 0) {          // front-right
        startAngle = 0.0f;
        endAngle   = 90.0f;
    } else if (corner == 1) {   // back-right
        startAngle = 90.0f;
        endAngle   = 180.0f;
    } else if (corner == 2) {   // back-left
        startAngle = 180.0f;
        endAngle   = 270.0f;
    } else {                    // front-left
        startAngle = 270.0f;
        endAngle   = 360.0f;
    }

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, yTop, cz);
    for (int i = 0; i <= segs; i++) {
        float theta = (startAngle + i * (endAngle - startAngle) / segs) * M_PI / 180.0f;
        float xPos = cx + radius * cos(theta);
        float zPos = cz + radius * sin(theta);
        glVertex3f(xPos, yTop, zPos);
    }
    glEnd();
}

    // ---------- TABLETOP SIDES ----------
    glColor3f(0.45f, 0.23f, 0.1f);

    // Draw curved sides (4 independent strips)
    for (int corner = 0; corner < 4; corner++) {
        float cx = (corner == 0 || corner == 3) ? innerX : -innerX;
        float cz = (corner < 2) ? innerZ : -innerZ;
        float startAngle, endAngle;

        if (corner == 0) { startAngle = 0.0f;   endAngle = 90.0f;  }  // front-right
        if (corner == 1) { startAngle = 90.0f;  endAngle = 180.0f; }  // back-right
        if (corner == 2) { startAngle = 180.0f; endAngle = 270.0f; }  // back-left
        if (corner == 3) { startAngle = 270.0f; endAngle = 360.0f; }  // front-left

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segs; i++) {
            float theta = (startAngle + i * (endAngle - startAngle) / segs) * M_PI / 180.0f;
            float xPos = cx + radius * cos(theta);
            float zPos = cz + radius * sin(theta);
            glVertex3f(xPos, yBottom, zPos);
            glVertex3f(xPos, yTop, zPos);
        }
        glEnd();
    }

    // ---------- FLAT SIDE WALLS ----------
    // front face
    glBegin(GL_QUADS);
    glVertex3f(-innerX, yBottom, innerZ + radius);
    glVertex3f( innerX, yBottom, innerZ + radius);
    glVertex3f( innerX, yTop,    innerZ + radius);
    glVertex3f(-innerX, yTop,    innerZ + radius);
    glEnd();

    // back face
    glBegin(GL_QUADS);
    glVertex3f(-innerX, yBottom, -innerZ - radius);
    glVertex3f( innerX, yBottom, -innerZ - radius);
    glVertex3f( innerX, yTop,    -innerZ - radius);
    glVertex3f(-innerX, yTop,    -innerZ - radius);
    glEnd();

    // left face
    glBegin(GL_QUADS);
    glVertex3f(-innerX - radius, yBottom, -innerZ);
    glVertex3f(-innerX - radius, yBottom,  innerZ);
    glVertex3f(-innerX - radius, yTop,     innerZ);
    glVertex3f(-innerX - radius, yTop,    -innerZ);
    glEnd();

    // right face
    glBegin(GL_QUADS);
    glVertex3f(innerX + radius, yBottom, -innerZ);
    glVertex3f(innerX + radius, yBottom,  innerZ);
    glVertex3f(innerX + radius, yTop,     innerZ);
    glVertex3f(innerX + radius, yTop,    -innerZ);
    glEnd();


    // ---------- LEGS ----------
    float legOffsetX = innerX;
    float legOffsetZ = innerZ;
    float legPositions[4][2] = {
        { legOffsetX,  legOffsetZ},
        {-legOffsetX,  legOffsetZ},
        {-legOffsetX, -legOffsetZ},
        { legOffsetX, -legOffsetZ}
    };

    glColor3f(0.35f, 0.2f, 0.1f);
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPositions[i][0], legHeight / 2, legPositions[i][1]);
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

void drawDoor(float x, float z, float width, float height)
{
    float yBottom = 0.0;
    float yTop = yBottom + height;
    float halfW = width / 2.0;

    // Door frame (optional, slightly darker)
    glColor3f(0.3, 0.18, 0.05);
    glBegin(GL_QUADS);
    // Left frame
    glVertex3f(x - halfW - 0.1, yBottom, z + 0.01);
    glVertex3f(x - halfW, yBottom, z + 0.01);
    glVertex3f(x - halfW, yTop, z + 0.01);
    glVertex3f(x - halfW - 0.1, yTop, z + 0.01);
    // Right frame
    glVertex3f(x + halfW, yBottom, z + 0.01);
    glVertex3f(x + halfW + 0.1, yBottom, z + 0.01);
    glVertex3f(x + halfW + 0.1, yTop, z + 0.01);
    glVertex3f(x + halfW, yTop, z + 0.01);
    glEnd();

    // Door panel
    glColor3f(0.45, 0.23, 0.1); // wooden door color
    glBegin(GL_QUADS);
    glVertex3f(x - halfW, yBottom, z);
    glVertex3f(x + halfW, yBottom, z);
    glVertex3f(x + halfW, yTop, z);
    glVertex3f(x - halfW, yTop, z);
    glEnd();

    // Door handle (small metallic knob)
    glColor3f(0.8, 0.8, 0.6);
    glPushMatrix();
    glTranslatef(x + halfW - 0.3, yBottom + height / 2.0, z + 0.05);
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

    float angleH = width / radiusH;   // horizontal angular span
    float angleV = height / radiusV;  // vertical angular span

    glPushMatrix();
    glTranslatef(wallX, yBase + halfHeight, wallZ);

    glColor3f(0.0, 0.3, 0.8); // main screen color
    glBegin(GL_QUADS);

    for (int i = 0; i < segH; i++) {
        float theta1 = -angleH / 2 + (angleH * i) / segH;
        float theta2 = -angleH / 2 + (angleH * (i + 1)) / segH;

        for (int j = 0; j < segV; j++) {
            float phi1 = -angleV / 2 + (angleV * j) / segV;
            float phi2 = -angleV / 2 + (angleV * (j + 1)) / segV;

            // spherical mapping
            float x1 = radiusH * sin(theta1);
            float z1 = -radiusH * cos(theta1) + radiusH + zOffset;

            float x2 = radiusH * sin(theta2);
            float z2 = -radiusH * cos(theta2) + radiusH + zOffset;

            float y1 = radiusV * sin(phi1);
            float y2 = radiusV * sin(phi2);

            // define quad corners
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y1, z2);
            glVertex3f(x2, y2, z2);
            glVertex3f(x1, y2, z1);
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
    float shadeTopRadius = 0.5f;
    float shadeHeight = 0.6f;

    glPushMatrix();

    // Lift so base is on floor
    glTranslatef(xPos, baseHeight, zPos);

    // ========== BASE (using triangles) ==========
    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2 * PI * i / segments;
        float theta2 = 2 * PI * (i + 1) / segments;
        float x1 = baseRadius * cos(theta1);
        float z1 = baseRadius * sin(theta1);
        float x2 = baseRadius * cos(theta2);
        float z2 = baseRadius * sin(theta2);

        // Triangle: center (0,0,0), (x1,z1), (x2,z2)
        glVertex3f(0, 0, 0);
        glVertex3f(x1, 0, z1);
        glVertex3f(x2, 0, z2);
    }
    glEnd();

    // ========== POLE ==========
    glColor3f(0.95f, 0.95f, 0.95f);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2 * PI * i / segments;
        float theta2 = 2 * PI * (i + 1) / segments;
        float x1 = poleRadius * cos(theta1);
        float z1 = poleRadius * sin(theta1);
        float x2 = poleRadius * cos(theta2);
        float z2 = poleRadius * sin(theta2);

        glBegin(GL_QUADS);
        glVertex3f(x1, 0, z1);
        glVertex3f(x2, 0, z2);
        glVertex3f(x2, poleHeight, z2);
        glVertex3f(x1, poleHeight, z1);
        glEnd();
    }

    // ========== SHADE ==========
    glColor3f(0.95f, 0.95f, 0.85f);
    float shadeBottomY = poleHeight;
    float shadeTopY = shadeBottomY + shadeHeight;

    for (int i = 0; i < segments; i++) {
        float theta1 = 2 * PI * i / segments;
        float theta2 = 2 * PI * (i + 1) / segments;
        float x1b = shadeBottomRadius * cos(theta1);
        float z1b = shadeBottomRadius * sin(theta1);
        float x2b = shadeBottomRadius * cos(theta2);
        float z2b = shadeBottomRadius * sin(theta2);
        float x1t = shadeTopRadius * cos(theta1);
        float z1t = shadeTopRadius * sin(theta1);
        float x2t = shadeTopRadius * cos(theta2);
        float z2t = shadeTopRadius * sin(theta2);

        glBegin(GL_QUADS);
        glVertex3f(x1b, shadeBottomY, z1b);
        glVertex3f(x2b, shadeBottomY, z2b);
        glVertex3f(x2t, shadeTopY, z2t);
        glVertex3f(x1t, shadeTopY, z1t);
        glEnd();
    }

    // ========== SHADE TOP (triangles, not fan) ==========
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float theta1 = 2 * PI * i / segments;
        float theta2 = 2 * PI * (i + 1) / segments;
        float x1 = shadeTopRadius * cos(theta1);
        float z1 = shadeTopRadius * sin(theta1);
        float x2 = shadeTopRadius * cos(theta2);
        float z2 = shadeTopRadius * sin(theta2);

        glVertex3f(0, shadeTopY, 0);
        glVertex3f(x1, shadeTopY, z1);
        glVertex3f(x2, shadeTopY, z2);
    }
    glEnd();

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

    // ==== Curved Backrest (starts from seat base) ====
    glColor3f(0.9f, 0.9f, 0.8f);
    float baseZ = -seatD/2;      // attach to back edge of seat
    float halfT = thickness / 2.0f;
    float backBaseY = seatY - thickness/2; // start from bottom of seat

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        float y1 = backBaseY + t1 * backHeight;
        float y2 = backBaseY + t2 * backHeight;

        float z1 = baseZ - curveDepth * (t1 * t1);
        float z2 = baseZ - curveDepth * (t2 * t2);

        glBegin(GL_QUADS);
        // front
        glVertex3f(-seatW/2, y1, z1 + halfT);
        glVertex3f(seatW/2,  y1, z1 + halfT);
        glVertex3f(seatW/2,  y2, z2 + halfT);
        glVertex3f(-seatW/2, y2, z2 + halfT);

        // back
        glVertex3f(-seatW/2, y1, z1 - halfT);
        glVertex3f(seatW/2,  y1, z1 - halfT);
        glVertex3f(seatW/2,  y2, z2 - halfT);
        glVertex3f(-seatW/2, y2, z2 - halfT);

        // left
        glVertex3f(-seatW/2, y1, z1 - halfT);
        glVertex3f(-seatW/2, y1, z1 + halfT);
        glVertex3f(-seatW/2, y2, z2 + halfT);
        glVertex3f(-seatW/2, y2, z2 - halfT);

        // right
        glVertex3f(seatW/2, y1, z1 - halfT);
        glVertex3f(seatW/2, y1, z1 + halfT);
        glVertex3f(seatW/2, y2, z2 + halfT);
        glVertex3f(seatW/2, y2, z2 - halfT);
        glEnd();
    }

    // top cap
    float yTop = backBaseY + backHeight;
    float zTop = baseZ - curveDepth;
    glBegin(GL_QUADS);
    glVertex3f(-seatW/2, yTop, zTop - halfT);
    glVertex3f(seatW/2,  yTop, zTop - halfT);
    glVertex3f(seatW/2,  yTop, zTop + halfT);
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
static float computeRadius(SceneObject* obj)
{
    float width  = obj->bbox[1] - obj->bbox[0];
    float depth  = obj->bbox[5] - obj->bbox[4];
    float radius = 0.5f * fmaxf(width, depth);
    return radius;
}

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
